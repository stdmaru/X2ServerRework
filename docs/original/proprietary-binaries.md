[Home](../README.md) › [Original code](README.md) › Proprietary binaries without source (`.lib` / `.dll`)

# Proprietary binaries without source (`.lib` / `.dll`)

These binaries cannot be ported. Remove each integration and, if game logic depends on its result, add an explicit stub.

Source: `#pragma comment(lib, ...)` and `.lib`/`.dll` files versioned in `_reference`.

## Server

| Binary | Purpose | Action |
|---|---|---|
| `GameServer/HackShield/AntiCpXSvr` | AhnLab anti-cheat | remove |
| `GameServer/nProtect/ggsrv*.lib`, `ggauth*.dll` | GameGuard anti-cheat | remove |
| `XTRAP/...xtrap4client_serverpart_*` | XTRAP anti-cheat | remove |
| `Common/Nexon/AuthCheck.lib` | Nexon auth | remove; implement own auth |
| `OnlyGlobal/AuthAndBilling/<country>/...` (`HanAuthForSvr`, `HanBillingForSvr`, `NetmarbleSignChecker`, PH `libeay32MD`...) | Publisher auth/billing | remove; retain only the direct DB auth flow |
| `MailMIME.lib/.dll`, `MailSMTP.lib/.dll` (copied into each server) | Email delivery (alerts?) | removed on 2026-10-31: without email, script error reports go to the log ([Common](../porting/server-common.md)) |
| `Common/CrashRpt/CrashRpt.lib` | Crash reporter | remove; use core dumps / sanitizers |
| `odbc32`, `ws2_32`, `winmm`, `dbghelp`, `psapi` | Windows system libraries | replaced by the platform layer |

## Client

| Binary | Purpose | Action |
|---|---|---|
| `d3d9`, `d3dx9`, `dxguid`, `dxerr9`, `dinput8`, `dsound`, `ddraw`, `dplayx` | DirectX | replace (see [Client](client.md)) |
| `fmodex.dll` | FMOD Ex audio (discontinued) | replace |
| `bdvid32.dll`, `bdcap32.dll` | Bandi video / capture | remove/replace |
| `KncFirewall.lib` | Windows firewall configuration | remove |
| `CrashRpt.lib`, `BugTrap(u).lib`, `knccrashrpt.dll` | Crash reporting | remove |
| `HShield` (`hsupchk`), `XTRAP`, `npgamelib*` | Anti-cheat | remove |
| `NexonModule`, `PurpleForClient`, `NMClientAuthDll`, `SSOWebLib`, `glbnmcashdll` | Publisher login/cash | remove |
| `steam_api`, `sdkencryptedappticket` | Steam | remove (or optional in the future) |
| `IGALib` | In-game ads | remove |
| `json_vc71_libmt` | Old jsoncpp | replace with upstream library |
| `zlibstat.lib`, `freetype2411.lib`, `libxml-md.lib`, `cximage/jpeg/png` | Precompiled open-source libraries | use upstream via CMake |
| `mfc100u`, `msvcr100`, `msvcp100` (in `_client/data`) | VS2010 runtime | will disappear |
