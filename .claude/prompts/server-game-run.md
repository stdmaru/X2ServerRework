Continue o porte do **GameServer** (Fase 2 do roadmap), passo **`server/game-run`**: o executável já compila sem warnings em Clang, GCC e
clang-cl (`server/game-build`, 2026-10-30). Agora ele tem de rodar e levar o cliente original da seleção de personagem até a vila.

Antes de tudo:
1. Leia o AGENTS.md e as páginas que ele manda ler (Roadmap e Decisões, até a T27).
2. Leia `docs/porte/server-game.md` (Estado, Mudanças, Pendências), e como modelo do passo equivalente `docs/porte/server-channel.md`
   (Estado, Executável) e `docs/ferramentas/runtime.md` (pastas `run/`, alvos `x2_run_*`, templates em `runtime/`).
3. Leia `docs/ferramentas/ambiente-build.md` (Docker, `tools/win/winbuild.sh`) e `docs/ferramentas/ci.md` (minutos).

Plano:
1. Levantar o que o GameServer precisa do LoginServer, do CenterServer e do GlobalServer para chegar à vila (`SendToLoginServer`, `ECN_*`,
   `SendToGlobalServer`, `KLoginProxy`/`KCnProxy`/`KGlobalProxy`): quais pacotes, em que momento do login, o que acontece se o servidor
   não existe. Provavelmente é preciso importar e portar o LoginServer e o CenterServer (cada um vira o seu `server/<x>-import`/`-build`).
2. `runtime/GameServer`: `config_gs_DEV.lua.in` derivado do `config_gs_US_SERVICE.lua` de produção (sem `CF_XTRAP`, sem HackShield),
   `.dsn.in` do `Game01`/`Account`/`Statistics`/`Billing`..., `x2_run_game` como o `x2_run_channel`.
3. Rodar contra o SQL Server do Docker e o EPublisherServer, com o cliente original no Wine: do "O servidor está cheio" (o canal aponta para
   `127.0.0.1:9300`) até a seleção de personagem e a vila. Conferir as pendências da página (catálogo do cash, grau de guilda, `unsigned long`).

Lições do `server/game-build` (2026-10-30), para não repetir:
- **Build do GameServer com `-j 6`**: com 16 jobs o GCC estoura a RAM (os `.cpp` de 10 a 15 mil linhas usam ~1,5 GB cada). Na VM, `-j 4`.
- `fix_warnings.py` comentava parâmetros dentro de macros (`IMPL_ON_FUNC`): agora pula `#define`; nelas use `[[maybe_unused]]`.
- `tools/port/fix_goto.py` sobe as declarações que um `goto end_proc` pula (DO_QUERY); confira os `const T*` no diff.
- Os flags do `ServerDefine.h` entram à força (`-include`) em todo arquivo do servidor: não dependa da ordem dos includes.
- O Clang (e o clang-cl) acha bugs que o GCC não acha (`-Warray-bounds`, `-Wnon-pod-varargs`, `-Wself-assign-field`): rode os dois.
- O código que parece descartável (publisher, auth, billing) pode ser o caminho ativo: confira a flag do config US antes de remover
  (o cash do US é o `SiKBillManager()`/DEB; a Nexon é código morto que ainda compila).
- No zsh, variável sem aspas não se divide em palavras; `pgrep -f`/`pkill -f` casam com o próprio shell: use PID.
- `git checkout <arquivo>` desfaz também as outras edições não commitadas do arquivo; `git rm` do último arquivo apaga a pasta.

Regras da sessão (AGENTS.md e memórias): `_reference/` só leitura (use `_reference/X2Server-github`; o link `X2Server` sumiu) e código
original só pelo importador; comentários em inglês, docs e commits em português; toda mudança de comportamento na página do módulo;
builds no Docker (`docker/build.sh`), Windows na VM (`tools/win/winbuild.sh`), CI só à mão nos branches (2.000 minutos/mês); commit e push
dos branches sem perguntar; integrar na `main` (ff-only, depois push) só quando o usuário pedir. Apague este arquivo no último commit do branch.
