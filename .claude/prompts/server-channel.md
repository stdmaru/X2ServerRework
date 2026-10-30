Comece o porte do **ChannelServer** (Fase 2 do roadmap). O branch `server/channel-import` já existe e tem só este arquivo.

Antes de tudo:
1. Leia o AGENTS.md e as páginas do wiki que ele manda ler (Roadmap e Decisões, até a T21).
2. Leia `docs/ferramentas/importador.md` e os módulos já feitos em `tools/import/modules/` (`server-common.toml`, `server-x2lib.toml`), que são o modelo do novo.
3. Leia `docs/porte/server-common.md` (Importação, Estado, Mudanças) e `docs/ferramentas/porte-mecanico.md`: o ChannelServer vai pela mesma passada.
4. Leia `docs/original/servidor.md` para o acoplamento do servidor com o Windows.

Plano, um branch curto por passo:
1. `server/channel-import`: `tools/import/modules/server-channel.toml` a partir do `KncWX2Server/ChannelServer/ChannelServer.vcxproj` (`Release_US`), com `--dry-run` antes. Só os arquivos próprios do ChannelServer (46 `.cpp`/`.h`, ~9 mil linhas de `.cpp`, contagem de 2026-10-30): o que é do `Common` já está em `src/server/Common`. Destino `src/server/ChannelServer` (mesma posição relativa do original, os fontes incluem `"../Common/..."`). Relatório em `tools/import/reports/`.
2. `server/channel-build`: compilar sem warnings em GCC, Clang e MinGW (o `Common` já compila nos três). `main` sem MFC: por enquanto o prompt Lua simples do KNCSDK (`lua_interpreter`, usado pelo `KBaseServer::Run`); o terminal da T15 e o log da T16 ficam para depois. Os hooks que hoje são stubs em `src/server/Common/tests/server_hooks.cpp` (`g_szConfigFile`, `CreateDBThread`) passam a vir do servidor. Página nova `docs/porte/server-channel.md` no índice do Porte.
3. `server/channel-run`: rodar no Linux com a configuração do `_reference/_original-server/ChannelServer` e conectar o `elsword.exe` original (`_client/`) até o fim do login no canal. Isso também cobre a pendência "validar o protocolo com bytes reais" do roadmap.

O que já existe para o passo 3:
- Banco: `docker compose -f docker/docker-compose.yml up -d --wait` e `docker/mssql/restore.sh` (bancos `Account`, `Game01`, `ES_BILLING`, `Statistics`; ver `docs/ferramentas/banco-dev.md`). Os `.dsn` do original usam outros nomes de banco (`Elsword-Account`...) e `DRIVER=SQL Server`: o `KODBC` troca o driver (`DBLayer:SetODBCDriver`, padrão ODBC Driver 18; o que está instalado é o FreeTDS, `/usr/lib/libtdsodbc.so`).
- Autenticação: o login usa o DEB (`_DEB_AUTH`, nunca descartável), cliente gSOAP que chama o `AuthServer` SOAP do dono do projeto, em `_reference/AuthServer` (tem `build-linux.sh`). Ele precisa estar rodando; ver como se configura antes de mexer.

Regras da sessão (também estão no AGENTS.md e nas memórias):
- `_reference/` é só leitura e o código original entra em `src/` só pelo importador. O link `_reference/X2Server` não existe: o importador usa `_reference/X2Server-github`. Nunca a cópia do HDD.
- Comentários em inglês; traduza os coreanos que tocar. Docs e commits em português.
- Toda mudança de comportamento em relação ao original vai para a página do módulo em `docs/porte/`.
- Builds: só o alvo afetado, incremental, `-j` livre. A matriz completa (GCC Debug/Release, Clang num diretório do scratchpad, MinGW + Wine) só antes de integrar.
- Ao terminar cada passo, faça o commit sem perguntar; ajustes vão por amend. Integre na `main` por fast-forward só quando o usuário pedir.
- Apague este arquivo (`.claude/prompts/server-channel.md`) no último commit do branch `server/channel-import`.
