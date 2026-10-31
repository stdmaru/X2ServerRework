Continue o porte do servidor (Fase 2 do roadmap) com o **GlobalServer**, o último dos servidores. Com o CenterServer (2026-10-31) o
cliente original vai da vila ao campo e à dungeon; o GameServer ainda descarta os pacotes `EGB_*` (CCU, missões globais, quest aleatória
do dia, bloqueio de usuário, registro de personagem), que vão ao GlobalServer (porta 9500). Nada trava sem ele, mas essas funções não existem.

Antes de tudo:
1. Leia o AGENTS.md e as páginas que ele manda ler (Roadmap e Decisões; a T28 é nova: scripts de produção no `ServerResource`).
2. Leia `docs/porte/server-center.md` como modelo mais recente (importação, Mudanças, Estado) e `docs/porte/server-game.md`
   ("Dependências de outros servidores").
3. Leia `docs/ferramentas/importador.md`, `docs/ferramentas/porte-mecanico.md`, `docs/ferramentas/runtime.md` (seções "Rodar no host" e
   "Scripts da pasta de cada servidor") e `docs/original/bugs.md`.

Estado em 2026-10-31: o CenterServer, o console UTF-8 do Windows, as decisões T29/T30 (ServerManager) e as pastas de execução no
build normal (`X2_RUN_FOLDERS`, `docker/.env`) já estão na `main`. O `server/global-import` está pronto (manifesto, código em
`src/server/GlobalServer`, `docs/porte/server-global.md`); siga do `server/global-build`.

Plano (um branch por passo):
1. **`server/global-import`**: `tools/import/modules/server-global.toml` (modelo: `server-center.toml`) para `KncWX2Server/GlobalServer`
   (34 `.cpp` no vcxproj, ~17 mil linhas no original). Nada excluído na importação além dos `.dsn`.
2. **`server/global-build`**: sem warnings em Clang, GCC (Debug e Release) e clang-cl (VM,
   `X2_WIN_HOST=Administrator@10.226.30.240 tools/win/winbuild.sh`), `main` com `KServerMain`, ícone do original.
3. **`server/global-run`**: `runtime/GlobalServer` (do `_reference/_original-server/GlobalServer`, `config_gb_US_SERVICE.lua`),
   `x2_run_global`, e os `EGB_*` do GameServer respondidos. Os `.lua` da pasta de produção já estão resolvidos pela T28
   (`WorldMissionConfig` no `ServerResource`; `GlobalMissionConfig` e `MissEliosConfig` não são citados por nada: conferir no código do Global).

Para testar, o usuário prefere rodar os servidores **direto no host** (sem Docker): configure uma vez com
`docker/build.sh cmake --preset linux-debug -DX2_RUN_ODBC_DRIVER=/usr/lib/libtdsodbc.so`, monte as pastas (`x2_run_*`) e rode cada um em
segundo plano com `cd build/linux-debug/run/<Servidor> && (sleep 7200) | ./<Servidor> DEV 0` (EPublisherServer sem o `0`). O GameServer
pode ir dentro do `gdb -batch -ex run -ex bt --args ...` para pegar *backtrace*. Para encerrar, feche o stdin matando o `sleep` irmão de
cada servidor pelo PID (não use `pkill -f` com um padrão que apareça no próprio comando). O usuário abre o cliente no Wine.

Lições do CenterServer (2026-10-31):
- O `fix_goto.py` entende arrays (`T a[N] = {0,};`); referências puladas pelo `goto` viram ponteiro declarado no topo.
- `-Woverloaded-virtual`: quando o `using` deixaria a chamada ambígua (parâmetro com valor padrão), renomeie a função escondida.
- Variável "set but unused" pode esconder chamada com efeito colateral (`GetKillNPC_LUA` sobe o `m_nKillNPC`): mantenha a chamada.
- O GCC Release acha `-Wmaybe-uninitialized` que o Debug não acha: rode a matriz completa antes de fechar o `-build`.
- Um branch que partiu antes de uma mudança na `main` testa com os dados velhos: rebaseie antes do `-run`.

Regras da sessão (AGENTS.md e memórias): `_reference/` só leitura (`_reference/X2Server-github`) e código original só pelo importador;
comentários em inglês, docs e commits em português, respostas ao usuário em português; toda mudança de comportamento na página do módulo;
bugs incertos em `docs/original/bugs.md`, os evidentes corrigidos já; builds no Docker, Windows na VM, CI só à mão; commit e push dos
branches sem perguntar. Apague este arquivo no último commit do branch.
