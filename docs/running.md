[Início](README.md) › Rodar os servidores

# Rodar os servidores

Como subir o conjunto de servidores e jogar com o cliente original. O passo a passo é o do Linux, o único em que os
servidores foram testados até 2026-10-31. Antes, compile: [Linux](building-linux.md) ou [Windows](building-windows.md).

Os comandos com `docker/build.sh` rodam no container de build. Num build nativo, rode o mesmo comando sem o prefixo.

## O que é preciso

- **Os backups do banco** em `_database/` (`Account.bak`, `Game01.bak`, `Statistics.bak`, `ES_BILLING.bak`).
- **O cliente original** em `_client/`: os servidores leem os scripts dele (gerados dos `.kom`), e é com ele que se joga.
  No Linux, o cliente roda pelo **Wine**.
- **Docker com `compose`**, para o SQL Server de desenvolvimento.

Nenhum deles está no repositório.

## 1. Banco de dados

Sobe o SQL Server 2022 em Docker e restaura os backups. O `restore.sh` **substitui** os bancos que já existirem:

```bash
docker compose -f docker/docker-compose.yml up -d --wait
docker/mssql/restore.sh
```

- Escuta só em `127.0.0.1:1433`, com o usuário `sa` e a senha de desenvolvimento do `docker/docker-compose.yml`.
- O `restore.sh` também ajusta os endereços dos servidores no banco para `127.0.0.1`, ou para o `X2_SERVER_IP`.
- Para parar: `docker compose -f docker/docker-compose.yml stop`. Os dados ficam no volume `x2-mssql-data`.

Detalhes: [Banco de desenvolvimento](ferramentas/banco-dev.md).

## 2. Configurações locais

Os padrões servem para tudo na mesma máquina. Para mudar alguma coisa, como a senha ou a porta do banco, o endereço que
os servidores anunciam, o driver ODBC ou a pasta do cliente, copie o modelo e descomente o que mudar:

```bash
cp docker/.env.example docker/.env
```

O `docker/.env` não é versionado. Ele é lido pelo `docker compose`, pelo `docker/build.sh`, pelo `restore.sh` e pelo CMake.
Uma variável definida no shell vence a do arquivo. As mudanças valem no próximo configure (`cmake --preset <preset>`).
O [`docker/.env.example`](../docker/.env.example) explica cada variável.

## 3. Pastas dos servidores

O build já monta, em `build/<preset>/run/`, uma pasta pronta para cada servidor:

```
build/linux-debug/run/
├── ServerResource/      scripts do servidor (do repositório)
├── ClientScript/        scripts gerados do cliente
├── EPublisherServer/    executável + config_*_DEV.lua + .dsn (já apontando para o banco)
├── ChannelServer/
├── LoginServer/
├── CenterServer/
└── GameServer/
```

Para montar só uma pasta: `docker/build.sh cmake --build --preset linux-debug --target x2_run_game`.
São um ambiente de desenvolvimento, não um pacote para outra máquina. Detalhes: [Pastas de execução](ferramentas/runtime.md).

## 4. Iniciar os servidores

Um terminal para cada servidor, cada um dentro da sua pasta. O primeiro argumento escolhe a configuração (`DEV` lê o
`config_*_DEV.lua`) e o segundo é o grupo de servidores:

```bash
cd build/linux-debug/run/EPublisherServer && ./EPublisherServer DEV
cd build/linux-debug/run/ChannelServer    && ./ChannelServer DEV 0
cd build/linux-debug/run/LoginServer      && ./LoginServer DEV 0
cd build/linux-debug/run/CenterServer     && ./CenterServer DEV 0
cd build/linux-debug/run/GameServer       && ./GameServer DEV 0
cd build/linux-debug/run/GlobalServer     && ./GlobalServer DEV 0
```

| Servidor | Porta | Função |
|---|---|---|
| EPublisherServer | 8000 | login e cash |
| ChannelServer | 9400 | primeiro contato do cliente: login, listas de servidores e canais |
| LoginServer | 9200 | contas, guildas, rankings |
| CenterServer | 9100 | salas de campo e de dungeon |
| GameServer | 9300 | o jogo em si |
| GlobalServer | 9500 | missões globais, grupo automático, PvP, contagem de usuários |

- As portas dos servidores de jogo vêm do banco (`Statistics.dbo.LServerList`); a do EPublisherServer, do `X2_RUN_EPUBLISHER_PORT`.
- A ordem não é rígida: o GameServer se reconecta a cada 10 s ao LoginServer, ao CenterServer e ao GlobalServer. O login só funciona com o
  EPublisherServer de pé.
- Cada servidor mostra no terminal um log compacto e colorido (`consolelog( n )` muda o nível dele) e grava o log completo num
  arquivo na pasta dele.
- O terminal tem um **prompt Lua**, como o campo de comando da janela do servidor original: o log corre acima dele sem quebrar a linha
  digitada, ↑/↓ percorrem o histórico e o título mostra `<Servidor>_Loading` até o estado pronto, depois `_Ready`.
- **Encerrar**: Ctrl+C ou Ctrl+D com a linha vazia pergunta `Are you sure you want to exit? [y/N]`; `y` encerra. SIGTERM encerra sem perguntar.
- **Sem terminal** (entrada por pipe ou arquivo), cada linha do stdin é um comando e o fim dele encerra o servidor.
  Para rodar como serviço ou em segundo plano, use `--no-stdin` (`./GameServer DEV 0 --no-stdin`): o servidor espera SIGTERM,
  SIGINT ou SIGHUP (no Windows, Ctrl+C ou o fechamento do console).
- **Um servidor já rodando** (em serviço, ou em outro terminal) atende pelo `run/x2ctl`: `x2ctl list`, `x2ctl run GameServer 'print(1)'`,
  `x2ctl stop GameServer`, ou `x2ctl attach GameServer` para o log e os comandos em JSON (também por `ssh`).
  Detalhes, opções (`--machine`, `--control=<nome>`) e o protocolo em [Console dos servidores](porte/console.md).
- Os executáveis compilados no container também rodam direto no host, numa distribuição com unixODBC e FreeTDS. Só o
  caminho do driver muda: `X2_RUN_ODBC_DRIVER` no `docker/.env`, por exemplo `/usr/lib/libtdsodbc.so` no Arch.

## 5. Cliente

Com os servidores no estado pronto, abra o cliente original (`_client/`; no Linux, `wine elsword.exe`) e faça login com
uma conta do banco. O banco de desenvolvimento tem a conta `test`, com a senha `test`.

O que já funciona (2026-10-31): login, seleção de personagem, vila, campo e dungeon, com os seis servidores. Algumas funções do
GlobalServer (missões globais, evento de encantamento, troca com limite) sobem sem dados, porque o banco de desenvolvimento não tem
as procedures delas ([Bugs do original](original/bugs.md#dados-serverresource-clientscript)).
