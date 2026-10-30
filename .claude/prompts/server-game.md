Continue o porte do **GameServer** (Fase 2 do roadmap). O código já foi importado (`server/game-import`, 298 arquivos em
`src/server/GameServer`); este branch, `server/game-build`, nasce dele e só tem este arquivo.

Antes de tudo:
1. Leia o AGENTS.md e as páginas que ele manda ler (Roadmap e Decisões, até a T23).
2. Leia `docs/porte/server-game.md` (importação, acoplamento medido, plano) e, como modelo do que foi feito no ChannelServer,
   `docs/porte/server-channel.md` e `docs/porte/server-common.md` (Mudanças).
3. Leia `docs/ferramentas/porte-mecanico.md` (`mechanical.py`, `fix_typename.py`, `fix_warnings.py`), `docs/ferramentas/runtime.md`
   (pastas `run/`, alvos `x2_run_*`, templates em `runtime/`) e `docs/ferramentas/ci.md`.

Plano:
0. **Antes do GameServer, `build/ubuntu-env`** (pedido do usuário em 2026-10-30): o ambiente principal passa a ser **Ubuntu**, o
   padrão global e o preferido pelo GitHub. Crie uma imagem Docker de build Ubuntu em `docker/` (LTS atual; GCC, Clang, CMake ≥ 3.28,
   Ninja, LuaJIT 2.1, zlib, unixODBC/FreeTDS, MinGW-w64 e Wine, Python), com um jeito curto de rodar os presets dentro dela
   (script ou `docker compose run`), e passe a fazer os builds e testes nesse container, não no host Arch. Ajuste o que o Ubuntu
   acusar (outra versão de GCC/Clang/CMake: warnings e erros novos). Mude o CI (`.github/workflows/ci.yml`, hoje num container
   `archlinux`, branch `build/ci`) para o mesmo ambiente Ubuntu (de preferência a mesma imagem). Registre a decisão em `decisoes.md`
   (T24), atualize `docs/ferramentas/ci.md`, o AGENTS.md (Comandos) e a T18 se o MinGW mudar. Integre só quando o usuário pedir.
   O `server/game-build` depois parte da `main` com isso (rebase).

   **Levantamento já feito (2026-10-30)**, com a `main` num container `ubuntu:26.04` (26.04.1 LTS: GCC 15.2, Clang 21.1, CMake 4.2.3,
   MinGW-w64 13 com GCC 13.2, Wine 10.0, LuaJIT `2.1.1761786044` do fork OpenResty):
   - GCC Debug e Clang Release: 0 erro, 0 warning, 6/6 testes (o 7º do Arch é a suíte `db`, que só existe com `X2_TEST_ODBC`).
   - GCC Release: 12 `-Wmaybe-uninitialized` em `KTCInfo::m_wstrUDPRelayIP`, só em `src/server/Common/tests/packet_test.h`
     (`CheckRoundTrip` de `KERM_OPEN_TC_ROOM_REQ` e `KEGS_CREATE_TC_ROOM_ACK`, que têm `T in{}`): falso positivo do GCC 15 com `std::wstring` no `-O2`
     (o GCC 16 não acusa). Tratar nos testes gerados (`tools/port/gen_packet_tests.py`), sem `-Wno` global.
   - MinGW: o padrão do Ubuntu é a variante `win32`; o `-posix` (winpthreads, como no Arch) precisa de `update-alternatives` na imagem
     ou de `X2_MINGW_TRIPLE`/compilador `-posix` no toolchain. Com GCC 13 os `.obj` dos pacotes (`ServerPacket.cpp`, `test_packets_*.cpp`)
     estouram o limite de seções do COFF ("file too big"): pôr `-Wa,-mbig-obj` no MinGW (e `/bigobj` no MSVC) em `cmake/X2Compiler.cmake`.
     Com `-posix` + `-Wa,-mbig-obj`: 0 erro, 0 warning, 6/6 no Wine.
   - LuaJIT: o pacote do Ubuntu é o fork OpenResty, não o upstream fixado na T21. Decidir: compilar o upstream fixo também no Linux
     (a receita do CMake da T21 já existe, falta uma opção para forçá-la) ou aceitar o pacote.
   - Wine em container: `WINEPREFIX` num diretório do usuário do container (`/root/.wine`) e `wineboot --init` antes do ctest.
   - Scripts do teste: `docker run -i ubuntu:26.04` com `git archive main` no stdin (o `_reference`, o `_client` e o `_database` ficam de fora).
   - O CI atual (`build/ci`, container `archlinux`) passou em GCC Debug/Release, Clang e checks; o MinGW só falhou no prefixo do Wine (corrigido).
1. `server/game-build` (este): `src/server/GameServer/CMakeLists.txt` (executável `x2_game_server`, `OUTPUT_NAME GameServer`, `main.cpp`
   chamando `KServerMain` como o ChannelServer; os hooks `g_szConfigFile`/`CreateDBThread` vêm do `GameServer.cpp`), passada mecânica,
   correção dos erros e zero warnings em GCC Debug/Release, Clang e MinGW. Remover as chamadas de anti-cheat (HackShield, nProtect,
   XTrap: 9 arquivos) e o TBB (`tbbmalloc_proxy.h`). É 15× o ChannelServer: se ficar grande, divida em branches em sequência.
   Atualize `docs/porte/server-game.md` (Estado, Mudanças) e o roadmap.
2. `server/game-run`: `runtime/GameServer` (config `config_gs_DEV.lua.in` derivado do `config_gs_US_SERVICE.lua` de produção, `.dsn.in`
   do `Game01`/`Account`/`Statistics`...), `x2_run_game`, e o cliente original passando do "O servidor está cheio" para a seleção de
   personagem e a vila. Antes, levante o que o GameServer precisa do LoginServer, do CenterServer e do GlobalServer
   (`SendToLoginServer`, `ECN_*`, `SendToGlobalServer`) para chegar à vila: provavelmente é preciso importar e portar alguns deles.

Lições do ChannelServer (2026-10-30), para não repetir:
- Literal `char` não ASCII que vá para rede, cripto ou hash: o fonte original é CP949 e o importado é UTF-8, então os bytes mudam
  (foi a causa da "falha na conexão com o servidor de canal", chaves do `KncSecurity`). Escreva em bytes.
- Caminhos com `\` (`"Pasta\\arquivo"`, `C:\\...`) viram nomes de arquivo no Linux: use `/`.
- `::exit()` chamado de uma thread de trabalho derruba o processo no meio dos destrutores estáticos; o ChannelServer passou a falhar o `Init`.
  Para falhas de memória, compile com ASan (`-fsanitize=address`) num diretório do scratchpad.
- O servidor se identifica no banco pelo IP (`SetLocalIP` no config; o `docker/mssql/restore.sh` aponta a `LServerList`/`LChannelList` para 127.0.0.1).
  Os dados do banco podem mudar por script; a estrutura (tabelas, procedures) não.
- Nada pode depender do `_reference/` (será apagado): o que o servidor lê vai para `runtime/` (scripts do servidor em produção) ou é gerado
  do cliente (`ClientScript`, `tools/runtime/gen_clientscript.py`). Ao comparar scripts, `tools/runtime/lua_trace.lua` compara por comportamento.
- O `ClientScript` gerado do cliente está no formato pós-processado (vetores como chamadas `D3DXVECTOR3(...)`), diferente do que o
  servidor original lia (texto). O GameServer é quem lê linemaps e diálogos: confira o `lua_tinker::get<D3DXVECTOR3>` ao rodar.
- `pkill -f <padrão>` casa com a linha do próprio shell; encerre processos pelo PID.

Regras da sessão (AGENTS.md e memórias): `_reference/` só leitura e código original só pelo importador (não reimporte módulo editado);
comentários em inglês, docs e commits em português; toda mudança de comportamento na página do módulo; builds só do alvo afetado,
matriz completa antes de integrar; commit e push dos branches sem perguntar (o CI do GitHub roda em todo push: `gh run list`);
integrar na `main` (ff-only, depois push) só quando o usuário pedir. Apague este arquivo no último commit do branch `server/game-build`.
