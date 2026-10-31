Continue o porte do servidor (Fase 2 do roadmap) com o **LoginServer**: sem ele o cliente original para em "Conectando ao servidor"
depois de conectar no GameServer (`server/game-run`, 2026-10-31). Com ele, o objetivo é o cliente da seleção de personagem até a vila.

Antes de tudo:
1. Leia o AGENTS.md e as páginas que ele manda ler (Roadmap e Decisões).
2. Leia `docs/porte/server-game.md` (Estado e "Dependências de outros servidores": quais pacotes o GameServer manda ao LoginServer
   no login), e como modelo `docs/porte/server-channel.md` e `docs/porte/server-game.md` (importação, build, Mudanças).
3. Leia `docs/ferramentas/importador.md`, `docs/ferramentas/runtime.md` e `docs/original/bugs.md` (bugs do original mantidos vão para lá).

Estado em 2026-10-31: o `server/game-run` está **integrado na `main`** (sem CI: os minutos acabaram até 2026-11-01; Windows conferido
na VM). Comece os branches a partir da `main`. Integrar só quando o usuário pedir; para pular o CI, `[skip ci]` no commit do topo.

Plano (um branch por passo, como o GameServer):
1. **`server/login-import`**: `tools/import/modules/server-login.toml` (modelo: `server-game.toml`) para `KncWX2Server/LoginServer`
   (45 `.cpp`, ~42 mil linhas). Ver o relatório; os `NexonAuth*`/`NexonBillingAuth*` e SMS/Mail são de publisher/legado.
2. **`server/login-build`**: compilar sem warnings em Clang e GCC (`-j 6`) e clang-cl (VM, `tools/win/winbuild.sh`, que usa todos os núcleos), `main` com `KServerMain`.
3. **`server/login-run`**: `runtime/LoginServer` (`config_lg_DEV.lua.in` do `_reference/_original-server/LoginServer`), `x2_run_login`.
   O login no GameServer passa por `ELG_NEXON_USER_AUTHENTICATE_REQ` (ramo `SERV_GLOBAL_AUTH`: `P_MUser_SEL_Auth` no `Account`) e
   `ELG_REGISTER_USER_REQ` → `ELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK`. Depois, a seleção de personagem e a vila (só bancos e o GameServer).

Para rodar tudo (cada servidor fica de pé enquanto o stdin estiver aberto):
`(sleep 3600) | docker/build.sh bash -c 'cd build/linux-debug/run/<Servidor> && ./<Servidor> DEV [0]'`, em segundo plano, para
EPublisherServer, ChannelServer, GameServer (e LoginServer). O usuário abre o cliente no Wine e faz o login.

Lições do `server/game-run` (2026-10-31):
- O LuaJIT recusa função com mais de 65.536 constantes; o `LUA_DOFILE` (`lua_dofile`) divide esses scripts. Erro de script novo:
  teste o arquivo com `build/linux-debug/_deps/luajit-src/src/luajit`.
- O `Authenticate` do `Common` portado devolve o que o `DBE_SERVER_ON_ACK` de cada servidor marca com `SetAuthenticateResult`:
  o LoginServer precisa marcar também (no lugar do `::exit( 0 )`).
- O Debug tem as asserções da libstdc++: leitura fora de vetor do original vira abort (achado nas skills de guilda).
- Erros de dados de produção ao carregar (itens, pets, dungeons que faltam) já existiam no original: anote em `docs/original/bugs.md`.
- `git rm` do último arquivo de uma pasta apaga a pasta: recrie antes de escrever nela.

Regras da sessão (AGENTS.md e memórias): `_reference/` só leitura (`_reference/X2Server-github`) e código original só pelo importador;
comentários em inglês, docs e commits em português; toda mudança de comportamento na página do módulo; builds no Docker
(`docker/build.sh`), Windows na VM, CI só à mão nos branches; commit e push dos branches sem perguntar. Apague este arquivo no último
commit do branch.
