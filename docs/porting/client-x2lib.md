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

## Status

Not compiled yet ([roadmap](../project/roadmap.md#phase-3--client-platform)).
