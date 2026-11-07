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

## Status

The header chain compiles (2026-11-07); the `.cpp` files do not yet
([roadmap](../project/roadmap.md#phase-4--client-graphics)).

Notes for the `.cpp` steps (2026-11-07):

- `CSystemInfo` (the machine ID) is still used by `X2Main.cpp`: the login packet needs it.
- `CX2Cursor`'s hardware cursor comes from an exe resource (`IDC_CURSOR1`); outside Windows it needs an SDL cursor.
- The `switch( GetPublisher() )` branches for other countries are dead.
