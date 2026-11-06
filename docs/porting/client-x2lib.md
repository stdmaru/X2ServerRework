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

## Status

Not compiled yet ([roadmap](../project/roadmap.md#phase-3--client-platform)).

To do when it is compiled (found on 2026-11-05, `client/serverprotocol`):

- `X2ServerEvent.h` declares the `EventID_Client.h` enumerators in its own `SERVER_EVENT_ID` enum (after `EVENT_X2_STARTUP =
  E_SYSTEM_EVENT_ID_END`), and the shared `Event.h` it now gets through `X2ServerProtocolLib.h` declares them too (same values,
  `E_CLIENT_EVENT_ID_BEGIN = E_SYSTEM_EVENT_ID_END`): a redefinition. Use the shared enum and keep `EVENT_X2_STARTUP` as a name
  for `E_CLIENT_EVENT_ID_BEGIN`. `CX2ServerEvent::SERVER_EVENT_ID_STR` is no longer needed by `CX2ServerProtocol`
  ([Client: X2ServerProtocol](client-serverprotocol.md)).
