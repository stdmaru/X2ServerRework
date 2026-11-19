[Home](../README.md) › [Porting](README.md) › Client: X2Lib

# Client: X2Lib (`src/client/X2Lib`)

The game layer of the client: states (`X2State*`), units and their skills (`X2GU*`, `X2GUUser`), NPCs, UI, items, quests, and
the glue to the network library (`X2Main`, `X2Data`). Uses Direct3D mostly as a math library and `HWND` through `MsgProc`
([analysis](../original/client.md)). Overview of the client port: [Client](client.md).

## Import (2026-11-05)

- Module `client-x2lib` ([report](../../tools/import/reports/client-x2lib/REPORT.md)): files of `X2Lib/` compiled by
  `X2Lib.vcxproj` (`US_INTERNAL|Win32`) and their headers: **552 files** (270 `.cpp`), 722k lines written; 27,462 `#if` groups
  resolved, 189k lines removed; 517 blocks became `X2_DEV_TOOLS` (now `X2_DEBUG`, T47); 472 include spellings fixed. Encoding: 405 CP949, 93 ASCII,
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
  - `[[maybe_unused]]` on locals and parameters that only `X2_DEBUG` code reads.
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

Anchors placed by the code (2026-11-13, `client/widescreen-migration`): the gauges of other players use one script on
both sides of the screen, so `CX2PartyMemberGageUI::SetPosition` anchors the party list left (and both classes anchor the members' buffs left), and
`CX2PVPPlayerGageUI::SetPosition` anchors each PvP team to the side it is placed on (x 6 left, x 868 right). The
item slots (`CX2SlotManager`) draw each item as a dialog of its own (`DLG_SlotManager.lua`, shared by every slot
window) placed by the code, so a slot manager has an anchor its items take: `CX2UIQuickSlot` sets it to its own
dialog's, and slot hit tests, dragging, tooltips and highlights go through `CX2Slot::GetCanvasPos`. The minimaps
(`CX2MiniMap`, `CX2FieldMiniMap`) draw the map at the frame's layout position (824, 558) outside the frame dialog, so
they draw it with the frame's anchor (`CX2MiniMapUI::GetFrameAnchor`); the map is 248 wide and runs 48 units past the
right edge under the frame's buttons, as in the original. The anchors of single-sided dialogs are in `UIAnchors.lua` ([client_data](../tools/client-data.md)).

Character select and creation (2026-11-13, `client/widescreen-screens`): the character stays centered, the lists and
the creation panel go right, and the info and exit buttons go left. Some statics mixed pictures from both sides, and anchors
are per control. Those pictures are statics of their own in the client_data, and the code shows them with the originals:
- `CX2StateServerSelect` shows the list labels `characListLabel` and `ChannelListLabel` with their lists.
- `CX2StateCreateUnit` shows the right half of the class info (`StaticUnit_NoChange_Unit_Info_Right`) and the create graph
  (`StaticUnit_Create_Unit_Graph`, the original's picture 3).
- `CX2StateCreateUnit::OnFrameMove` keeps the daily creation notice (`Creative_Character_Notice`, centered) shown with the
  panel (`Creative_Character`).

The NPC talk shade (`CX2TFieldGame::NpcJoinShadow`) is two 2D particles from `UIMajorParticle.txt` (`JoinNpcTop`,
`JoinNpcBottom`), 1024 wide and centered on the layout's middle. On a wide canvas they darkened only the middle column.
`NpcJoinShadow::OnFrameMove` now sizes them to the canvas width every frame (2026-11-13, `client/widescreen-screens`).

Known, left for the migration: screens whose texture does not match its rectangle's aspect stay as the original drew
them. The character select background, for example, is a 512×512 texture drawn at 1024×768, so it is stretched 33% in
the original as well.

## NPC marks follow the resolution (2026-11-14, `tools/ui-editor`)

`CX2TFieldNpc` puts its marks (quest `!`/`?`, event, name) at the NPC's projected head plus offsets that were pixels at
1024×768 (`+60,+30` for quests, `-50,+20` for events, `-37`/`-40` for the name), while the sprites themselves are scaled
with the resolution (`ConvertSizeByResolution` in the particle system). On a small window the marks drifted away from the
NPC; on a large screen they crowded it (user, 2026-11-14). The offsets now go through `ConvertSizeByResolution` too, so
every resolution looks as 1024×768 did. Other 2D marks placed with pixel offsets from a projected point may need the same.

## HUD gauge shake (2026-11-14, `tools/ui-editor`)

`CX2MyGageUI::CrashMyStateGageUI` shakes the player's portrait and bars on a hit. The original moved the dialog to
random points and back to (0, 0), its script's position; it now shakes around the dialog's own position, so a script
that moves it keeps it there (the client_data puts it at (-2, -2) to reach the screen edges,
[client_data](../tools/client-data.md#content-changes-t42)). With the original script it behaves the same.

## Field map scroll (2026-11-14, `client/widescreen-map-pan`)

`CX2WorldFieldMap::CalcPosForScroll` moves the "M" field map with the character: at 4:3 from 0 (the map's left end on
screen) to the far end of its dragging range. The range now shrinks on a wider screen
([KTDXLIB](client-ktdxlib.md#panned-pictures-cover-the-screen-2026-11-14-clientwidescreen-map-pan)), so both ends move:
it goes between them instead of from 0, which at 4:3 is the same. Before, the map's left end sat at the centered
column's edge while the character was in the left part of a field, leaving the screen's left strip uncovered (user,
2026-11-14).

The navigation box in the map's corner (`CX2WorldMapUI`: clicking or dragging it moves the map, and it follows the
map) took the map's position from the layout's 0 and was 4:3 wide. With the narrower range, dragging it put the map out
of range, the range pulled it back and the box jumped when released (user: "glitches"). The box now stands for the
screen: its left top maps to the canvas's (`NaviToMapPos` / `MapToNaviPos`), its 4:3 size grows with the canvas within
the background (`WidenNaviWindow`), and the map is kept in range at once. At 4:3 it is the original's.

## State names (2026-11-14, `tools/ui-editor`)

`CX2Main::StateName` gives a state's `XS_*` name; `StateInLog` writes it, and dev builds pass it to the UI anchor
recorder at each state change (the [UI editor](../tools/ui-editor.md)'s scenes). Change from the original: the class
change and arcade states were logged as `XS_INVALID` (missing from `StateInLog`'s switch) and now log their own names.

## Items from the loose scripts (2026-11-15, `server/add-character`)

`CX2ItemManager::OpenItemScriptFile` loads `Item.kim` from the archives and reads `Item.lua` + `SetItem.lua` only when
that fails. Debug and dev builds now read the two scripts first when both are loose files of the
[client_data](../tools/client-data.md) (`IsValidFile_LocalFile`), so an item added there shows without a pack: Add's
basic items were in `Item.lua` but not in the packed `Item.kim`, and creation showed him with no model. Loading takes
longer (two passes over a 59 MB script). LuaJIT allows 65,536 constants in a function and `Item.lua` (51,593
`AddItemTemplet` calls) has more, so those builds run the loose scripts in pieces of 500 top-level statements
(`DoItemScript`). Release builds read `Item.kim` as before.
`KFileSerializer::AppendWideString`'s `std::wstring` overload, which builds that table, still copied the 4-byte
`wchar_t` while every reader expects UTF-16 (model names read as "M"); it now writes UTF-16 as the pointer overload
([KTDXLib](client-ktdxlib.md)).

## Special skill speech balloon (2026-11-16, `client/cutin-talk-elesis-add`)

`CX2GUUser::GetCutInFileName` picks the balloon (`HQ_CutIn_Talk_<L|R>_<name>_0n.dds`) shown with the portrait when a
special active skill is used. The original had no case for Elesis or Add: the name ended in `_`, the texture failed to
open and `CutIn_Talk_*` was drawn with the texture still bound, the portrait, so a second copy of the character appeared
where the balloon goes. Fix: Elesis uses her own balloons (`Elesis`, `Saber_Knight` also for Grand Master,
`Pyro_Knight` also for Blazing Heart); Add, who has none in the assets, and any class without a case get the generic
`HQ_CutIn_Talk_<L|R>_<0..2>.dds` ("Toma essa!", ...).

**Behavior change (widescreen):** every portrait (`HQ_CutIn_*.dds`) has art touching its texture's left edge, meant to
bleed off the screen. `CX2GUUser::ActiveSkillShow` slid the portrait in to the 4:3 edges, 0 and 1024; on the wide
canvas, which extends past both, the edge showed as a cut through the portrait (clear on Add and Elesis, whose art is wide
there). The slide-in is now anchored to `GetUICanvasLeft`/`GetUICanvasRight`, the same at 4:3. The balloons
(`CutIn_Talk_*`, placed by `GameMinorParticle.txt`) stay in the 1024 column. The portrait facing left was also cut in
half by depth clipping of the mirrored quad, a renderer bug ([D3D9](client-d3d9.md#how-a-draw-is-translated)).

## Tutorial pointer placements (2026-11-16, `client/widescreen-migration`, [T52](../project/decisions.md))

The tutorial's click pointer (`CX2State::MouseClickUI`, `DLG_Mouse_Click.lua`) was put at 13 literal layout points in
four files. `CreateMouseClickUI` now takes the name of one of the script's placements
([KTDXLIB](client-ktdxlib.md#named-placements-2026-11-16-clientwidescreen-migration-t52)); the arrow style and its
±10 bounce stay in the code, moved with the placement.

| Placement | Called from | Was | Placement in the script |
|---|---|---|---|
| `GUIDE_QUEST_MENU` | `CX2PlayGuide` step 0 | (783, 233) | fixed, `RIGHT` (by the quest tracker, `DLG_UI_NEW_Quest_Quick.lua`, of which there are several) |
| `GUIDE_EPIC_QUEST_TAB` | step 1 | (135, 150) | `DLG_UI_NEW_Quest_List.lua` + (135, 150) |
| `GUIDE_EPIC_QUEST_LIST` | step 2 | (300, 135) | `DLG_UI_NEW_Quest_List.lua` + (300, 135) |
| `GUIDE_QUEST_COMPLETE` | step 3 | (230, 515) | `DLG_UI_NEW_Quest_Detail.lua` / `Button_Complete` + (230, 515) |
| `GUIDE_QUEST_REWARD` | step 4 | (220, 460) | `DLG_UI_NEW_Quest_Clear.lua` (at 100, 100) + (120, 360) |
| `GUIDE_DUNGEON_MENU` | step 5 | (872, 125) | `DLG_UI_PARTY_BOTTON_NEW.lua` (`RIGHT`) + (872, 125) |
| `GUIDE_EL_TREE` | step 6 | (525, 435) | `DLG_UI_Local_Map_NEW.lua` (at 216, 132) + (309, 303) |
| `GUIDE_DUNGEON_APPLY` | step 7 | (960, 474) | `DLG_UI_PARTY_LENGTH_NEW.lua` (`RIGHT`) + (960, 474) |
| `GUIDE_FOREST_RUIN` | step 8 | (390, 620) | `DLG_UI_Local_Map_NEW.lua` + (174, 488) |
| `SUMMON_CARD_SLOT` | `CX2UIQuickSlot`, `CX2DungeonGame` (defence dungeon) | (107, 763) | `DLG_UI_Quick_Slot.lua` (`LEFT`) + (107, 763) |
| `DUNGEON_ROOM` | `CX2StateDungeonRoom` (×2, quest 11) | (805, 215) | fixed, no target found |

The targets were deduced from the code and the scripts (2026-11-16; the recording has no tutorial screens): what each
step asks the player to click, and the dialog that has it. Offsets are the original literal less the target's
position when the step runs, so at 4:3 the pointer is where the original put it. **Behavior change:** the quest
windows are placed by `CX2UIManager` (top left, or beside another window open there), and the pointer now follows
them; the original's points were right only when the quest window was the first. Checked by the user in the game
(2026-11-16): works as expected.

## Scene loading screen (2026-11-16, `client/scene-loading`)

**Behavior change** (user, 2026-11-16; no `src/define.h` switch, the user's call: a quality-of-life upgrade). Entering a
village or a field builds the world at once but leaves its meshes and textures to the device manager's loading thread
([D3D9](client-d3d9.md), background loading); with the upscaled textures the first frames showed the character floating
in an empty world. The original covered this only after a warp or a channel change (`CX2TFieldGame::LoadUI`, the
area's pictures from `DLG_PVP_Game_Loading_State.lua`) and on boss raid fields (`CX2BattleFieldGame::CreateLoadingUI`),
both for a fixed 3 s whatever was loaded.

`CX2SceneLoading` (`X2SceneLoading.h/.cpp`, owned by `CX2Main`) is now shown on every entry: `CX2Main::StateChange` to
`XS_VILLAGE_MAP` or `XS_BATTLE_FIELD` (character select, village ↔ field, dungeon or PvP back to the village, channel
change) and `CX2StateBattleField::MoveFromBattleFieldToOtherBattleField` (field to field, no state change). It uses the
first login's layout, `DLG_First_Loading_Front.lua` (its background and bottom bar, the client version hidden), over a
black fill of the whole canvas, and presents one frame at once, before the state's synchronous loading. It is
`CKTDXApp`'s overlay stage ([KTDXLIB](client-ktdxlib.md#overlay-stage-and-load-counters-2026-11-16-clientscene-loading)),
moved and drawn after every stage, so it outlives the state change and covers everything the new state draws.

It closes when, together and for 0.15 s and 3 drawn frames:

- the device manager's background loads are idle (everything queued since the screen opened finished, nothing new);
- the state's entry is confirmed: the village sent `EGS_FIELD_LOADING_COMPLETE_REQ` (on its first frame) and has its
  ACK (the dungeon lounge has no packet); the field has `EGS_GAME_LOADING_ACK`;
- it has been up at least 0.5 s.

The scene is drawn behind it all along, so when it fades out (0.3 s) the uploads were submitted and the pipelines
built. After 30 s it closes anyway, with a line in the log. It closes at once if the state changes to one it does not
cover (a disconnection). The bar (and `STR_ID_655`, "loading n%") shows finished over queued background loads for 85 %
of its length and the server's confirmation for the rest; it never goes back.

The two timed screens give way to it: `LoadUI` keeps only the warp's 3 s delay of the field and buff names and spends
the warp/channel flags; the raid field no longer calls `CreateLoadingUI` (removed). Input is not blocked meanwhile.

**Village to village and the area's name** (2026-11-18): a village to another one (fast travel, warp ticket) keeps
`XS_VILLAGE_MAP` and only rebuilds `g_pTFieldGame` in `CX2StateField::Handler_EGS_STATE_CHANGE_FIELD_ACK`, so
`StateChange` never raised the screen; that handler now calls `BeginSceneLoading` before the rebuild. The area's name
(village: `m_fFieldNameTime`, `m_fBuffNameTime` in `CX2TFieldGame::OnFrameMove`; field: `OpenBattleFieldName`) is a timed
overlay that used to start with the game's construction and ran out behind a long load. The village's timers now stand
still while `CX2Main::IsSceneLoading()`, and the field opens its name (`m_bBattleFieldNamePending`) on the first frame
after the screen is gone.

**Backgrounds** (user, 2026-11-16): every destination uses the same dialog; only the textures of `First_Loading_Back`
change, chosen at `Begin` (the location and field managers already know the destination; the channel change flag is
spent only while the village is built). The table in `X2SceneLoading.cpp` holds the art of the original's two timed
screens, now shown on every entry, not only after a warp: the 8 villages and 6 rest fields (`LoadFieldImage`, moved
there from `CX2TFieldGame`), the channel change (`Channel_A`/`Channel_B`) and the boss raid fields
(`HQ_LoadingField_Boss_Gate_01/02`, the original's one picture for every raid). Other destinations (the ordinary
fields, the dungeon lounge) draw one of the generic `HQ_LoadingBasic_*.tga` at random (user, 2026-11-16; 1024×512
textures made to be stretched to 4:3, as the dungeon loading screen draws them; 12 distinct, the byte-identical
`HQ_LoadingBasic`/`_2` = `_1` and `_111_2` = `_111_1` left out). An entry is either two 512×768 halves of a 4:3
image or a single picture, 4:3 (over the 1024 column) or, for the 16:9 redraws planned under
[T51](../project/decisions.md) (one texture each), 1365.33 units wide centered on the column; the second picture is
then hidden (or absent, once the dialog's own background is one 16:9 picture). If a texture is missing the dialog is
rebuilt with its own background, the first login's.

## Dungeon stage loading screen (2026-11-18, `client/dungeon-stage-loading`)

The screen between a dungeon's stages (`CX2StateDungeonGame::OnFrameRender`, `m_NextStageNum != -2`) is a dialog,
`DLG_Dungeon_Stage_Loading.lua` (`Static_Stage_Loading`, both pictures `STRETCH` in `UIAnchors.lua`), with two layers:

0. **Back**: the stage's last frame without the UI; until it is taken, or if it cannot be, a generic loading picture
   (`Dungeon_Loading_Advertisement.lua`).
1. **Frame**: `HQ_LoadingOutLine.tga`, the black bands with the logo (stretched to the screen's shape: the logo is wider
   at 16:9).

The shot had been lost: the original copied the back buffer with `StretchRect` into a 1024×768 target, and the device has
no such copy ([render targets](client-ktdxlib.md#render-targets)), so it showed black. `CX2Game::TakeLastKillShot` now
draws the scene (world and object chain, before the UI) **into the target** on the first render after the last kill's
delay (`m_bLastKillShotDue`, set by `OnFrameMove`), the target sized as the back buffer (reopened when it changes), so the
shot is at the screen's resolution. The picture shows the target through `CPictureData::SetRenderTarget`, opaque (blend
one / zero). The first stage's screen (`DLG_PVP_Game_Loading_State.lua`) is unchanged.

## Characters waiting for deletion (2026-11-16, `client/unit-wait-delete`)

A deleted character waits in character select (`SERV_UNIT_WAIT_DELETE`) with the permanent deletion (`final_delete_unit`)
and restore (`restore_unit`) buttons, each enabled from the date the server sends (`m_trDelAbleDate`,
`m_trRestoreAbleDate`, from `GUnitWaitDelete`). Fixed in `CX2StateServerSelect::CreateServerSelectUnitViewerUI`:

- **The deletion date is shown.** The original commented out the code filling `StaticUnitSelectDeleteInfo` (in
  `DLG_UI_Character_Selection_Back_New.lua`) with `STR_ID_16109` ("you can delete this character permanently on …"),
  so the player saw two disabled buttons and no date. It is back, wrapped at 300 units. **Behavior change.**
- **The sad motion plays every time.** `m_bDeletedMotionPlay` (the waiting motion, `UVUMT_WAIT_DELETE`, was started on
  the current viewer) belongs to the unit and outlived the viewer: the `REFORM_ENTRY_POINT` path deletes the old viewer
  without clearing it (the older path did), so a deleted character selected again stood still. The new viewer clears it.

The dates themselves come from the database ([`20261116_unit_wait_delete_dates.sql`](../tools/database-patches.md#game01)):
permanent deletion after 24 hours (at once on admin accounts, to test characters), restore at once.

## Village movement sync (2026-11-19, `server/village-sync`, [T53](../project/decisions.md))

Under `X2_VILLAGE_SYNC` (`src/define.h`) the village replaces the original's 2 s input replay
([Village movement sync](../original/village-sync.md)) with the states the server validates
([Server: Common](server-common.md#intentional)). **Behavior change.** Squares (`CX2SquareGame`) use it too since
2026-11-19 (their 5 s update still goes, for the ED it carries, and only that is read from it). The dungeon lounge
keeps the original's sync: it is a CenterServer room not yet on the new pipeline (`CX2TFieldGame::m_bStateSync`, false
when `CX2InstanceData::GetIsDungeonLounge`); wedding halls (`CX2WeddingGame`) are untouched. Both games keep a
`CX2UnitStateSync` (`X2UnitStateSync.*`), which does what follows; a unit asks its game's for the clock and the world.

- **Sending** (`CX2UnitStateSync::FrameMove`, every 0.1 s on average, keeping what is left of each interval, instead of `SyncUnitFrame` every 2 s):
  `EGS_FIELD_UNIT_STATE_NOT` with the own unit's position, physics speeds, last line, state ID and change count, and
  facing (`CX2SquareUnit::GetFieldUnitState`), only when one of them changed, plus once after it stops so the others
  see where. The tick is a steady millisecond clock (`GetClockMs`; `GetTickCount` steps by 10 to 16 ms on
  Windows) that always advances.
- **Other units** (`EGS_FIELD_UNIT_STATE_BATCH_NOT` → `ReceiveStateBatch` → `CX2SquareUnit::PushFieldUnitState`): kept
  in a `KFieldUnitSnapshots` and drawn at the server's time less 200 ms (`CX2UnitStateSync::GetRenderTick`, with
  `KFieldServerClock`). Each frame `UpdateRemotePose` sets the interpolated position, facing, line and speeds, and
  enters the sender's state when it changes (by ID and change count); its animation follows from the state, as
  before. Such a unit runs no physics (`PhysicProcess`: only the facing and landing point from its line,
  `UpdateDirFromLine`) and no state events, which read the keys: a state its own frame function enters (an animation
  that ended) is entered again only if the sender re-entered it. Emotions are not entered from a state: they come
  with the chat message, which plays them (`PlayEmotion`); a state only leaves them. A party member not drawn still
  gets its position on the party list, as with the original's sync. A unit is drawn from its first state on
  (`m_bFirstGetPacket`); the field sends a unit's state as soon as it comes into view, and while the unit's models
  load (`AddUnitToDeviceList` creates it later) its newest state waits in `m_mapPendingState` (`OnUnitAdded`), since a unit standing
  still sends no other.
- **Corrections** (`EGS_FIELD_UNIT_CORRECTION_NOT` → `ReceiveCorrection` → `CX2SquareUnit::ApplyFieldCorrection`): the
  own unit goes back to the last state the server accepted, standing (`WAIT` or `RIDING_WAIT`; in the air it falls
  from there), the reason is logged, and the next update reports it.
- **Talking to an NPC** (`CX2TFieldGame::TalkNpc`): the field hides every unit and stops their frames while the
  dialog is open, the own unit too, so one that jumped stayed in the air, where the server rejects it, and was rolled
  back when the dialog closed. Now the own unit first stops on the ground below it, standing
  (`CX2SquareUnit::StopOnGround`); hidden, the fall is not seen. The keys stay off as before.
- `TCP_NODELAY` on the client's connections to its servers (`KSession::Connect`; the TCP relay already had it).

Not checked in game yet (2026-11-19): riding pets, emotions, invisibility and personal shops with two clients.

## Status

All 265 `.cpp` files compile warning-free (2026-11-11), and the client links on them
([Client: X2](client-x2.md#x2cpp-2026-11-12-clientx2-link), 2026-11-12).

Notes for linking (2026-11-07):

- `CSystemInfo` (the machine ID) is still used by `X2Main.cpp`: the login packet needs it.
- The `switch( GetPublisher() )` branches for other countries are dead.
