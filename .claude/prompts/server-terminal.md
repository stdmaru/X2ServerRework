Continue a Fase 2 do roadmap com o **terminal dos servidores (T15) e o log estruturado (T16)**. Em 2026-10-31 todos os servidores do
original estão portados (EPublisher, Channel, Login, Center, Game, Global) e rodam juntos no Linux com o cliente original. A janela MFC
do original foi trocada por um prompt Lua simples no stdin (`lua_interpreter` do KNCSDK, chamado por `KBaseServer::Run`), e o log vai
para o console como texto pronto.

Antes de tudo:
1. Leia o AGENTS.md e as páginas que ele manda ler (Roadmap e Decisões). Atenção à **T15 revisada**, à **T16** e à **T30** (2026-10-31):
   o servidor não terá interface gráfica nenhuma; a janela vira o ServerManager, uma ferramenta à parte que usa um modo de saída para
   máquina e um socket local (`x2ctl attach`). Não reabra essas decisões.
2. Leia `docs/original/servidor.md` (a linha da UI MFC: `Common/ui/{ServerApp,ServerUIDlg,EditLog,HookStdio}`), `docs/porte/kncsdk.md`
   (`lua_interpreter`, `dbg/logfile`), `docs/porte/platform.md` (`console.h`: UTF-8 e cores no Windows, e a limitação de leitura de UTF-8
   pelo stdin no conhost) e `docs/porte/server-common.md` (`KServerMain`, `BaseServer::Run`).
3. Leia o comportamento do original em `_reference/X2Server-github/KncWX2Server/Common/ui/` (só leitura): o que a janela mostrava, o histórico
   do campo de comando, o título `_Loading`/`_Ready`, a confirmação ao fechar.

Plano (um branch por passo; ajuste depois de ler o código):
1. **`kncsdk/log-structured`** (T16): o `START_LOG`/`BUILD_LOG` entrega ao logger os metadados (nível, função, hora, campos, arquivo e
   linha) em vez de texto pronto. O arquivo mantém o conteúdo completo do original. O console ganha uma renderização própria, compacta e
   colorida (`HH:MM:SS NÍVEL Função: mensagem`, campos alinhados abaixo), com nível de filtro independente do arquivo. São ~13.500 usos das
   macros: a mudança é na implementação delas, não nos pontos de chamada.
2. **`server/terminal`** (T15): o terminal no lugar do `lua_interpreter`: log ao vivo sem quebrar a linha que está sendo digitada, prompt
   Lua com histórico, status `_Loading`/`_Ready` no título do terminal, confirmação ao encerrar (Ctrl+C/Ctrl+D) e modo sem stdin para
   serviço. No Windows, leitura com `ReadConsoleW` (o UTF-8 pelo stdin não é confiável no conhost). Escolha uma biblioteca de edição de
   linha só se valer a pena (T13: `find_package` primeiro, `FetchContent` com versão fixa como reserva); senão, implementação própria em
   `x2::platform`.
3. Depois (não agora): o modo para máquina e o socket local da T30, que o ServerManager vai usar.

Para testar, rode os servidores **direto no host** (sem Docker): o build normal já monta `build/linux-debug/run/<Servidor>`
(`X2_RUN_FOLDERS`), e o driver ODBC do host vem do `docker/.env` (`X2_RUN_ODBC_DRIVER=/usr/lib/libtdsodbc.so`) ou do cache. Rode cada
um em segundo plano com `cd build/linux-debug/run/<Servidor> && (sleep 7200) | ./<Servidor> DEV 0` (EPublisherServer sem o `0`); para
testar o terminal interativo, o usuário abre um terminal de verdade. Para encerrar, feche o stdin matando o `sleep` irmão de cada
servidor pelo PID (não use `pkill -f` com um padrão que apareça no próprio comando). O Windows compila e testa na VM com
`tools/win/winbuild.sh` (`X2_WIN_HOST` pode vir do `docker/.env`).

Regras da sessão (AGENTS.md e memórias): `_reference/` só leitura; comentários em inglês, docs e commits em português, respostas ao usuário
em português; toda mudança de comportamento na página do módulo; builds no Docker, Windows na VM, CI só à mão (os minutos do Actions
voltam em 2026-11-01); commit e push dos branches sem perguntar, integração na `main` só quando o usuário pedir. Apague este arquivo no
último commit do último branch.
