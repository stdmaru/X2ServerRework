Continue o porte do servidor (Fase 2 do roadmap) com o **CenterServer**: com o LoginServer (2026-10-31) o cliente original chega à
vila, mas ao sair para o campo para no `ERM_OPEN_BATTLE_FIELD_REQ`, que o GameServer manda ao CenterServer (porta 9100). Com ele, o
objetivo é o cliente no campo e numa dungeon.

Antes de tudo:
1. Leia o AGENTS.md e as páginas que ele manda ler (Roadmap e Decisões).
2. Leia `docs/porte/server-login.md` como modelo mais recente (importação, Mudanças, Estado) e `docs/porte/server-game.md`
   ("Dependências de outros servidores": o que o GameServer manda ao Center).
3. Leia `docs/ferramentas/importador.md`, `docs/ferramentas/porte-mecanico.md`, `docs/ferramentas/runtime.md` e `docs/original/bugs.md`.

Estado em 2026-10-31: os branches `server/login-import`, `server/login-build` e `server/login-run` estão um sobre o outro, com push,
**ainda não integrados na `main`** (pergunte ao usuário se integra antes de começar; sem CI até 2026-11-01, `[skip ci]` no commit do topo).
Comece o próximo branch a partir do `server/login-run` (ou da `main`, se já integrado).

Plano (um branch por passo, como o LoginServer):
1. **`server/center-import`**: `tools/import/modules/server-center.toml` (modelo: `server-login.toml`) para `KncWX2Server/CenterServer`
   (62 `.cpp`, ~77 mil linhas no original). Nada excluído na importação; publisher, país e KSMS saem no build.
2. **`server/center-build`**: sem warnings em Clang, GCC (`-j 6`: os `.cpp` grandes usam ~1,5 GB cada) e clang-cl (VM,
   `X2_WIN_HOST=Administrator@10.226.30.240 tools/win/winbuild.sh`), `main` com `KServerMain`.
3. **`server/center-run`**: `runtime/CenterServer` (do `_reference/_original-server/CenterServer`), `x2_run_center`, e o cliente no campo
   e numa dungeon. O GameServer acha o Center pela lista do banco (`Statistics.LServerList`) e reconecta a cada 10 s.

Para rodar tudo (cada servidor fica de pé enquanto o stdin estiver aberto), em segundo plano:
`(sleep 7200) | docker/build.sh bash -c 'cd build/linux-debug/run/<Servidor> && ./<Servidor> DEV [0]'` para EPublisherServer (sem o 0),
LoginServer, ChannelServer, GameServer (e CenterServer). Para encerrar, não use `pkill -f` com um padrão que apareça no próprio comando.
O usuário abre o cliente no Wine e faz o login.

Lições do LoginServer (2026-10-31):
- O FreeTDS devolve `SQL_NO_DATA` para procedures sem resultado; o `KODBC::Query` agora conta isso como sucesso (o driver do Windows
  da produção fazia igual). Os logs de produção em `_reference/_original-server/<Servidor>/log` ajudam a decidir o que é do original.
- `DBE_SERVER_ON_ACK` de cada servidor precisa marcar `SetAuthenticateResult( true/false )` no lugar do `::exit( 0 )`.
- Os erros que mais aparecem: includes que o MSVC dispensava nos templates (`BaseServer.h`, `ActorManager.h`), `goto end_proc` sobre
  declarações (`tools/port/fix_goto.py` com log do GCC: `docker/build.sh cmake --build build/gcc-debug ...`), destrutores sem `virtual`
  nos singletons, `virtual inline ProcessEvent` (o clang-cl acusa).
- O `build/linux-release` pode ter cache de fora do Docker: `cmake --preset linux-release --fresh`.
- `git rm` do último arquivo de uma pasta apaga a pasta: recrie antes de escrever nela.

Regras da sessão (AGENTS.md e memórias): `_reference/` só leitura (`_reference/X2Server-github`) e código original só pelo importador;
comentários em inglês, docs e commits em português; toda mudança de comportamento na página do módulo; bugs mantidos em
`docs/original/bugs.md`; builds no Docker, Windows na VM, CI só à mão; commit e push dos branches sem perguntar. Apague este arquivo no
último commit do branch.
