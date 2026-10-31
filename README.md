# X2ServerRework

Porte do servidor (e, mais adiante, do cliente) de **Elsword** para **Windows e Linux**, em 64 bits, com CMake.
O código original é só Windows (MFC, DirectX 9, MSVC 32 bits). Aqui ele é trazido para C++20, compilado com Clang
nas duas plataformas e limpo no caminho, mantendo o **protocolo compatível com o cliente original**: os servidores
portados funcionam com o `elsword.exe` de sempre, sem mudar nada nele nem nos assets.

> Este README é a introdução: o que é, como compilar e como rodar. Os detalhes (decisões, andamento, notas de cada
> módulo, análise do código original) ficam no [wiki do projeto](docs/README.md).

## Estado (2026-10-31)

| Servidor | Função | Estado |
|---|---|---|
| EPublisherServer | login e cash (substitui o serviço de autenticação externo) | roda no Linux |
| ChannelServer | login do cliente, lista de servidores e canais | roda no Linux |
| LoginServer | contas, guildas, rankings, tudo o que é do grupo de servidores | roda no Linux |
| GameServer | personagens, vila, campos, dungeons, itens, quests, PvP | roda no Linux |
| CenterServer | salas de campo e de dungeon | roda no Linux |
| GlobalServer | missões globais, quest do dia, contagem de usuários | em porte |

Com eles, o cliente original faz login, escolhe o personagem, entra na vila, vai ao campo e entra numa dungeon.
No Windows, tudo compila sem warnings e os testes passam; os servidores ainda não foram testados rodando lá.
O que vem depois (o GlobalServer, o terminal novo dos servidores, o ServerManager e o porte do cliente) está no
[Roadmap](docs/projeto/roadmap.md).

## O que é preciso

**Para compilar no Linux**: Docker. O build roda numa imagem Ubuntu do próprio projeto (Clang, GCC, CMake, Ninja,
unixODBC, FreeTDS), a mesma do CI; o host não precisa de mais nada.

**Para compilar no Windows**: Visual Studio 2026 (ou o Build Tools) com a carga **C++** e os componentes
**Clang** (clang-cl) e **CMake**. As dependências (zlib, LuaJIT, Asio, doctest) são baixadas pelo próprio CMake.

**Para rodar**:
- um **SQL Server** com os bancos do jogo (`Account`, `Game01`, `Statistics`, `ES_BILLING`): o projeto sobe um em Docker
  e restaura os backups de `_database/` nele;
- o **cliente original** em `_client/`: os servidores leem os scripts do cliente (gerados a partir dos `.kom`), e é com ele que se joga.
  No Linux o cliente roda pelo Wine.

Os backups do banco e o cliente não fazem parte do repositório.

## Compilar

### Linux

```bash
docker/build.sh cmake --preset linux-debug
docker/build.sh cmake --build --preset linux-debug
docker/build.sh ctest --preset linux-debug
```

`docker/build.sh` roda o comando dentro da imagem de build (sem argumentos, abre um shell nela) e a constrói na primeira vez.
Os presets do Linux:

| Preset | Compilador | Tipo | Ferramentas de dev |
|---|---|---|---|
| `linux-debug` | Clang | Debug | sim |
| `linux-development` | Clang | RelWithDebInfo | sim |
| `linux-release` | Clang | Release | não |
| `linux-gcc` | GCC | Release | não (verificação) |

"Ferramentas de dev" são os comandos de debug e de administração do original (`X2_DEV_TOOLS`); fora deles, o build é o
de produção. Mais detalhes em [Ambiente de build](docs/ferramentas/ambiente-build.md).

### Windows

Num *Developer Command Prompt* (ou *Developer PowerShell*) do Visual Studio, com o clang-cl no `PATH`
(`<Visual Studio>\VC\Tools\Llvm\x64\bin`):

```bat
cmake --preset windows-debug
cmake --build --preset windows-debug
ctest --preset windows-debug
```

Os presets são os mesmos do Linux: `windows-debug`, `windows-development` e `windows-release`.

## Rodar os servidores

1. **Banco de dados.** Sobe o SQL Server de desenvolvimento e restaura os backups (substitui os bancos, se já existirem):

   ```bash
   docker compose -f docker/docker-compose.yml up -d --wait
   docker/mssql/restore.sh
   ```

   Ele escuta em `127.0.0.1:1433`; a senha de desenvolvimento está no `docker/docker-compose.yml` e muda com `X2_DB_PASSWORD`.
   Ver [Banco de desenvolvimento](docs/ferramentas/banco-dev.md).

2. **Pastas de execução.** Cada alvo `x2_run_<servidor>` compila o servidor e monta a pasta dele em `build/<preset>/run/`,
   com os configs já apontando para o banco e os scripts (`ServerResource` do repositório e `ClientScript` gerado do cliente):

   ```bash
   docker/build.sh cmake --build --preset linux-debug --target x2_run_epublisher x2_run_channel x2_run_login x2_run_center x2_run_game
   ```

3. **Servidores.** Um terminal por servidor, cada um na sua pasta. O primeiro argumento é a configuração (`DEV`
   lê o `config_*_DEV.lua`) e o segundo é o grupo de servidores:

   ```bash
   cd build/linux-debug/run/EPublisherServer && ./EPublisherServer DEV
   cd build/linux-debug/run/ChannelServer    && ./ChannelServer DEV 0
   cd build/linux-debug/run/LoginServer      && ./LoginServer DEV 0
   cd build/linux-debug/run/CenterServer     && ./CenterServer DEV 0
   cd build/linux-debug/run/GameServer       && ./GameServer DEV 0
   ```

   O GameServer se reconecta sozinho ao LoginServer e ao CenterServer, então a ordem não é rígida; o EPublisherServer
   precisa estar de pé para o login.

4. **Cliente.** Abra o cliente original (`_client/`; no Linux, pelo Wine) e faça login com uma conta do banco.

Cada servidor mostra o log no terminal (e grava o completo em arquivo) e aceita **comandos Lua** pela entrada, como o
campo de comando da janela original. **O fim da entrada (Ctrl+D no Linux, Ctrl+Z e Enter no Windows) encerra o servidor.**

Os servidores compilados na imagem também rodam direto no host, numa distribuição com unixODBC e FreeTDS; só muda
o caminho do driver ODBC na configuração (`-DX2_RUN_ODBC_DRIVER=...`). Isso e todas as outras opções das pastas de
execução estão em [Pastas de execução](docs/ferramentas/runtime.md).

## Layout do repositório

```
src/            código: platform/ (camada de sistema), kncsdk/ (biblioteca base), server/ (servidores)
runtime/        o que os servidores leem ao rodar: ServerResource e os templates de configuração
cmake/          módulos CMake do projeto
docker/         imagem de build e SQL Server de desenvolvimento
tools/          importador do código original, utilitários do cliente (.kom), build do Windows por SSH, wiki
docs/           wiki do projeto
```

## Contribuir

Quem for mexer no código deve começar pelo [AGENTS.md](AGENTS.md) (regras do repositório: estilo, branches, o que não
se muda) e pelo [wiki](docs/README.md), principalmente o [Roadmap](docs/projeto/roadmap.md) e as
[Decisões](docs/projeto/decisoes.md).
