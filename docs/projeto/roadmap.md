[Início](../README.md) › [Projeto](README.md) › Roadmap

# Roadmap

Ordem pensada para ter algo **testável** o mais cedo possível. A peça-chave: o cliente original
(`_client/elsword.exe`, PE32 x86) já existe e funciona — então o servidor portado pode ser validado
contra ele **antes** de tocarmos no cliente. Isso só funciona se mantivermos o protocolo de rede
idêntico ([Protocolo de rede e serialização](../original/protocolo.md)).

```
Fase 0  Repo novo + importador automático       (copia do _reference já limpo; sem mudar lógica)
Fase 1  Bibliotecas: substituir 3rdparty, portar KNCSDK
Fase 2  Servidor  → Linux/Windows, testado com o cliente original
Fase 3  Cliente   → camada de plataforma (SDL) + abstração gráfica
Fase 4  Cliente   → backend gráfico novo (Vulkan / SDL_GPU)
Fase 5  Limpeza profunda / reorganização
```

## Fase 0 — Repo novo + importador

O `_reference` **não é modificado** (fica só leitura). O código novo nasce do zero em `src/`, mas os arquivos
que mantemos **não são copiados à mão**: um script (`tools/import/`) lê o `_reference` e grava em `src/`
a versão já limpa. Motivo em [decisoes.md](decisoes.md) (T9).

O importador, por etapa:

1. **Seleção de arquivos**: lê os `.vcxproj` na config `Release_US|Win32` e pega só os fontes que realmente compilam
   (respeitando `ExcludedFromBuild`) + os headers que eles incluem. Arquivo que não entra no build não é importado.
2. **Encoding**: CP949 → UTF-8, fim de linha LF. Ver [Codificação de arquivos e convenções do código original](../original/codificacao.md).
3. **Feature flags congeladas** com `coan` usando o conjunto final de macros da variante US/`_SERVICE_`
   (~1.175 flags `SERV_*`, ~58 mil linhas de `#if*`). Ver [Feature flags e variantes](../original/feature-flags.md).
   **Exceção:** macros de debug/dev da whitelist e as checagens diretas de ambiente (`#ifndef _SERVICE_`, `_IN_HOUSE_`)
   viram `#if X2_DEV_TOOLS` e entram no relatório para triagem. Ver [Ferramentas de debug / desenvolvimento / administração](../original/debug-e-dev.md).
4. **Exclusões**: lista explícita de pastas/arquivos a não trazer (outros países, anti-cheat, SOAP, binários proprietários, DXUT...).
5. **Mapa de destino**: caminho no `_reference` → caminho em `src/` (onde a reorganização de pastas acontece).
6. Relatório: arquivos importados, descartados e flags resolvidas.

Regras:
- Correções que só um humano faz (portar IOCP, trocar tipos, etc.) acontecem **depois** da importação, em commits normais em `src/`.
- O importador roda **por módulo** (primeiro KNCSDK, depois `Common`, depois cada servidor), conforme cada fase precisa — não importamos tudo de uma vez.
- Re-rodar o importador sobre um módulo já editado não é permitido; se descobrirmos um erro de flag depois, corrigimos à mão em `src/`.
  (Antes da primeira edição manual de um módulo, re-rodar é livre.)
- A unificação de duplicatas (`KNCSDK_Server`×`KNCSDK_Client`, as duas pastas `Socket`, as 3 cópias de `lua_tinker`) é feita no mapa
  de destino: escolhe-se uma fonte e as diferenças da outra viram revisão manual.

Tarefas:
- [x] Layout do repo (`src/`, `cmake/`, `tools/import/`), CMake raiz, presets Linux/Windows x64, `.gitattributes`, `.editorconfig`.
- [x] Importador ([../../tools/import/README.md](../ferramentas/importador.md)): seleção via vcxproj, encoding, resolvedor de `#if` próprio
      (no lugar do `coan`, por causa da regra `X2_DEV_TOOLS`), exclusões, mapa de destino, correção de includes, relatório, trava contra sobrescrita.
- [x] Módulo `kncsdk` importado: 62 arquivos de 742 ([relatório](../../tools/import/reports/kncsdk/REPORT.md)).
- [x] Flags do **servidor** geradas a partir do `ServerDefine.h` (736 verdadeiras / 423 falsas): [Flags e verificação](../ferramentas/flags.md).
- [x] Resolvedor validado em escala por verificação diferencial: 1.415/1.423 arquivos do servidor equivalentes (os 8 restantes são os próprios headers de configuração).
- [x] Importador confere a integridade de cada arquivo contra o git (a cópia do HDD tinha 90 arquivos corrompidos, ver [Inventário](../original/inventario.md#integridade-da-cópia)).
- [ ] Flags do **cliente** (`KTDX.h`/`Always.h`) e conflitos entre servidor e cliente no código compartilhado (`Common`): fazer junto com a importação do cliente.
- [x] `server-common` importado: 414 arquivos ([Servidor: Common](../porte/server-common.md)).
- [x] `server-x2lib` importado: `X2Lib/X2UDPPacket.*` (structs UDP do cliente que o servidor compila), em `src/server/X2Lib` (2026-10-29).
- [x] `server-channel` importado: 36 arquivos ([Servidor: ChannelServer](../porte/server-channel.md), 2026-10-30).
- [x] `server-game` importado: 298 arquivos ([Servidor: GameServer](../porte/server-game.md), 2026-10-30).
- [x] `server-login` importado: 86 arquivos ([Servidor: LoginServer](../porte/server-login.md), 2026-10-31).
- [ ] Importar os outros servidores (Center, Global).

## Fase 1 — Bibliotecas

A maior parte não é "portar", é **substituir pela versão upstream** via CMake. Só o KNCSDK é código próprio.
Tabela completa: [Bibliotecas](../original/bibliotecas.md).

Andamento em 2026-10-29: plataforma e KNCSDK compilando no Linux (GCC e Clang) e no MinGW-w64 (testes no Wine), com testes. Detalhes em [Porte](../porte/README.md).

- [x] Camada de plataforma `x2::platform` (tipos, sync, tempo, `CTime`, `CString`, CRT, unicode), com testes. Ver [Plataforma](../porte/platform.md).
- [x] Dependências via CMake (T13): zlib, LuaJIT 2.1 (pkg-config, ou fonte com commit fixo pelo Makefile dele: T21), doctest.
- [x] KNCSDK portado: log, `Serializer` (formato de rede fixo), `KncSecurity`, `Thread`, `ObjectPool`, `KncUtil`, unido ao `KNCSDK_Client`. Ver [KNCSDK](../porte/kncsdk.md).
- [x] `lua_tinker` (versão do servidor) compilando em GCC/Clang sobre LuaJIT 2.1; luabind removido.
- [ ] **Validar o protocolo com bytes reais** do cliente/servidor originais. Em 2026-10-30 o login no canal funcionou com o `x2_DEBUG.exe` e o `x2.exe` (troca de chave, cripto, `ECH_VERIFY_ACCOUNT`, listas); em 2026-10-31 o cliente conectou no GameServer (troca de chave, `EGS_VERIFY_ACCOUNT_REQ`); falta o resto.
- [x] Compilar e testar os caminhos Windows com **MinGW-w64 + Wine** (T18): plataforma e KNCSDK sem warnings, testes passando (2026-10-29).
- [x] LuaJIT 2.1 no MinGW (T21): `kncsdk_lua` e o `Common` do servidor compilando sem warnings e com os testes passando no Wine (2026-10-30).
- [x] Crypto++ substituído por `x2::crypto` (DES-CBC, HMAC-MD5, MD5, SHA-256), com vetores oficiais (T27, 2026-10-30).
- [x] Ambiente de build único: imagem Docker Ubuntu 26.04 para builds locais e CI (T24, `build/ubuntu-env`, 2026-10-30). Ver [Ambiente de build](../ferramentas/ambiente-build.md).
- [ ] Compilar e testar no **Windows/MSVC** e LuaJIT no MSVC (D5).
- [ ] Boost: o KNCSDK não usa mais; o servidor ainda usa em ~230 arquivos. Reduzir ao portar o servidor.
- [ ] Intel TBB 2.2 → `std` (2 usos, no GameServer).
- [ ] libxml2 e FreeType upstream: só o cliente usa (Fase 3).

## Fase 2 — Servidor

Plano detalhado do `Common`: [Servidor: Common › Plano](../porte/server-common.md#plano-do-porte-manual-um-branch-por-passo).

Ver [Servidor — acoplamento com Windows](../original/servidor.md).

- [x] `server/common-build`: `Common` compilando no Linux (GCC/Clang) sem warnings, menos `Socket/`/`ODBC/` (2026-10-29). Ver [Servidor: Common](../porte/server-common.md).
- [x] `server/common-packets`: ida e volta de todos os 1.964 tipos serializados + bytes exatos do início do login (2026-10-29). Achou 2 `GET` que chamavam `PUT` no original.
- [x] `server/common-net`: TCP de `Socket/` sobre Asio (T20), com teste de `KSession` por loopback (2026-10-29).
- [x] `server/common-udp`: UDP de `Socket/` (`KTDNUDP`, `UdpEcho`, `LBSUdpEcho`) sem completion routines, com teste por loopback (2026-10-29).
- [x] `server/common-db`: ODBC portável (T17), testado contra SQL Server 2022 em Docker com os bancos do jogo, via FreeTDS (2026-10-30): [Banco de dados (ODBC)](../porte/server-common-db.md). Driver 18 ainda não testado.
- [x] `server/channel-build`: ChannelServer compilando sem warnings (GCC, Clang, MinGW), `main` sem MFC (`KServerMain`), 2026-10-30. Ver [Servidor: ChannelServer](../porte/server-channel.md).
- [x] EPublisherServer: reimplementação do AuthServer do DEB (T23), testada com o cliente gSOAP dos servidores e contra o banco (2026-10-30). Ver [Servidor: EPublisherServer](../porte/server-epublisher.md).
- [x] `server/channel-run`: ChannelServer no Linux com o `elsword.exe` original até o fim do login no canal (2026-10-30): login pelo EPublisherServer, listas de grupos de servidores e de canais recebidas; o cliente então tenta o GameServer (porta 9300), que ainda não existe, e mostra "O servidor está cheio". Pasta `run/` pelo CMake (T22).
- [x] `server/game-build`: GameServer compilando sem warnings (Clang, GCC, clang-cl), `main` sem MFC, sem anti-cheat nem TBB (2026-10-30). Ver [Servidor: GameServer](../porte/server-game.md).
- [x] `server/game-run`: GameServer no Linux até o estado pronto, com o cliente original conectando nele (2026-10-31): o "servidor cheio" some e o login para no `EGS_VERIFY_ACCOUNT_REQ`, que depende do LoginServer ([levantamento](../porte/server-game.md#dependências-de-outros-servidores-levantamento-de-2026-10-31)).
- [x] `server/login-import`: LoginServer importado (2026-10-31). Ver [Servidor: LoginServer](../porte/server-login.md).
- [x] `server/login-build`: LoginServer compilando sem warnings (Clang, GCC, clang-cl), sem a Nexon (KR) nem os arquivos de país (2026-10-31). Ver [Servidor: LoginServer](../porte/server-login.md).
- [ ] `server/login-run`: LoginServer rodando; com ele, o cliente da seleção de personagem até a vila.
- [ ] UI MFC (`Common/ui/ServerUIDlg`) → terminal em todas as plataformas (T15): log ao vivo, prompt Lua com histórico, título com status, encerramento com confirmação, modo serviço sem stdin.
- [ ] Log (T16): metadados estruturados no `START_LOG`; arquivo com o conteúdo completo do original, console compacto e colorido com nível próprio.
- [ ] (Futuro) `--gui` portável sobre a mesma API de comandos/log.
- [ ] Remover SOAP/gSOAP, billing/auth de publishers, Mail(MIME/SMTP), CrashRpt; stub onde a lógica de jogo depende.
- [ ] Marco: login + vila + dungeon com o `elsword.exe` original contra servidor rodando em Linux.

## Fase 3 — Cliente: plataforma

Ver [Cliente — acoplamento com Windows / Direct3D 9](../original/cliente.md).

- [ ] Janela/input/IME: Win32 + DirectInput → SDL3. Manter, por enquanto, o modelo `MsgProc(HWND, UINT, WPARAM, LPARAM)`
      traduzindo eventos SDL em mensagens no estilo Win32 (X2Lib usa `HWND` em ~272 arquivos — quase todo pela assinatura de `MsgProc`).
- [ ] Matemática D3DX (`D3DXVECTOR3` ~8.000 usos) → shim header-compatível sobre GLM ou implementação própria.
- [ ] Som: FMOD Ex / DirectSound → miniaudio/SDL3 audio (ou FMOD Core atual, que é multiplataforma).
- [ ] Vídeo (Bandi), browser embutido, CxImage, Steam, anti-cheat → remover/substituir.
- [ ] `.kom` (zlib + header "KOG GC TEAM MASSFILE V.0.3") — loader portável.

## Fase 4 — Cliente: gráfico

- [ ] Introduzir abstração gráfica dentro de `KTDXLIB` (D3D9 está concentrado lá: ~56 arquivos usam o device; X2Lib quase só usa matemática).
- [ ] Reimplementar o que hoje vem de D3DX: loader `.x` (mesh + skin + animação), efeitos `.fx` (`x2.fx`), texturas `.dds`, fontes, sprites.
- [ ] Backend novo: SDL_GPU ([decisoes.md](decisoes.md), T19).

## Fase 5 — Limpeza profunda

Refatoração de verdade (remoção de sistemas mortos, organização de pastas, nomes), guiada por testes/smoke tests
das fases anteriores. Só depois que tudo roda.

- [ ] Consertar os [bugs do original](../original/bugs.md) mantidos no porte, cada um com a decisão do comportamento novo.
