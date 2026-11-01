[Início](../README.md) › [Código original](README.md) › Binários sem fonte (`.lib` / `.dll`)

# Binários sem fonte (`.lib` / `.dll`)

Não podem ser portados. Para cada um: remover a integração e, se a lógica de jogo depender do resultado, deixar um stub explícito.

Fonte: `#pragma comment(lib, ...)` + `.lib/.dll` versionados no `_reference`.

## Servidor

| Binário | Função | Ação |
|---|---|---|
| `GameServer/HackShield/AntiCpXSvr` | anti-cheat AhnLab | remover |
| `GameServer/nProtect/ggsrv*.lib`, `ggauth*.dll` | anti-cheat GameGuard | remover |
| `XTRAP/...xtrap4client_serverpart_*` | anti-cheat XTRAP | remover |
| `Common/Nexon/AuthCheck.lib` | auth Nexon | remover; auth própria |
| `OnlyGlobal/AuthAndBilling/<país>/...` (`HanAuthForSvr`, `HanBillingForSvr`, `NetmarbleSignChecker`, `libeay32MD` PH...) | auth/billing de publishers | remover; manter só fluxo de auth direto no DB |
| `MailMIME.lib/.dll`, `MailSMTP.lib/.dll` (copiados em cada servidor) | envio de e-mail (alertas?) | removido em 2026-10-31: sem e-mail, o relatório de scripts com erro vai para o log ([Common](../porting/server-common.md)) |
| `Common/CrashRpt/CrashRpt.lib` | crash reporter | remover; usar core dump / sanitizers |
| `odbc32`, `ws2_32`, `winmm`, `dbghelp`, `psapi` | sistema Windows | substituídos pela camada de plataforma |

## Cliente

| Binário | Função | Ação |
|---|---|---|
| `d3d9`, `d3dx9`, `dxguid`, `dxerr9`, `dinput8`, `dsound`, `ddraw`, `dplayx` | DirectX | substituir (ver [client.md](client.md)) |
| `fmodex.dll` | áudio FMOD Ex (descontinuado) | substituir |
| `bdvid32.dll`, `bdcap32.dll` | Bandi video / captura | remover/substituir |
| `KncFirewall.lib` | config de firewall do Windows | remover |
| `CrashRpt.lib`, `BugTrap(u).lib`, `knccrashrpt.dll` | crash report | remover |
| `HShield` (`hsupchk`), `XTRAP`, `npgamelib*` | anti-cheat | remover |
| `NexonModule`, `PurpleForClient`, `NMClientAuthDll`, `SSOWebLib`, `glbnmcashdll` | login/cash de publishers | remover |
| `steam_api`, `sdkencryptedappticket` | Steam | remover (ou opcional no futuro) |
| `IGALib` | in-game ads | remover |
| `json_vc71_libmt` | jsoncpp antigo | trocar por lib upstream |
| `zlibstat.lib`, `freetype2411.lib`, `libxml-md.lib`, `cximage/jpeg/png` | libs abertas pré-compiladas | upstream via CMake |
| `mfc100u`, `msvcr100`, `msvcp100` (no `_client/data`) | runtime VS2010 | desaparecem |
