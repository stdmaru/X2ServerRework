[Home](README.md) › Run the servers

# Run the servers

How to start the server suite and play with original client. Steps below are for Linux, the only platform on which servers had been
tested as of 2026-10-31. Build first: [Linux](building-linux.md) or [Windows](building-windows.md).

Commands prefixed with `docker/build.sh` run in build container. For native build, run same command without prefix.

## Requirements

- **Database backups** in `_database/` (`Account.bak`, `Game01.bak`, `Statistics.bak`, `ES_BILLING.bak`).
- **Original client** in `_client/`: servers read its scripts (generated from `.kom`), and it is used to play.
  On Linux, client runs through **Wine**.
- **Docker with `compose`**, for development SQL Server.

None of these are in the repository.

## 1. Database

Start SQL Server 2022 in Docker and restore backups. `restore.sh` **replaces** existing databases:

```bash
docker compose -f docker/docker-compose.yml up -d --wait
docker/mssql/restore.sh
```

- Listens only on `127.0.0.1:1433`, user `sa`, with development password from `docker/docker-compose.yml`.
- `restore.sh` also changes server addresses in database to `127.0.0.1` or `X2_SERVER_IP`.
- Stop with `docker compose -f docker/docker-compose.yml stop`. Data remains in `x2-mssql-data` volume.

Details: [Development database](tools/development-database.md).

## 2. Local settings

Defaults work when everything is on same machine. To change something (e.g. password, DB port, advertised server address,
ODBC driver, or client folder), copy template and uncomment values to change:

```bash
cp docker/.env.example docker/.env
```

`docker/.env` is not versioned. It is read by `docker compose`, `docker/build.sh`, `restore.sh`, and CMake.
Shell variable takes precedence over file. Changes apply on next configure (`cmake --preset <preset>`).
[`docker/.env.example`](../docker/.env.example) describes each variable.

## 3. Server directories

Build assembles ready-to-run directory for each server under `build/<preset>/run/`:

```text
build/linux-debug/run/
├── ServerResource/      server scripts (from repository)
├── ClientScript/        scripts generated from client
├── EPublisherServer/    executable + config_*_DEV.lua + .dsn (already pointed at DB)
├── ChannelServer/
├── LoginServer/
├── CenterServer/
└── GameServer/
```

To assemble only one: `docker/build.sh cmake --build --preset linux-debug --target x2_run_game`.
These are development environments, not packages for another machine. Details: [Run directories](tools/runtime.md).

## 4. Start servers

Use one terminal per server, each from its own directory. First argument selects config (`DEV` reads `config_*_DEV.lua`), second
is server group:

```bash
cd build/linux-debug/run/EPublisherServer && ./EPublisherServer DEV
cd build/linux-debug/run/ChannelServer    && ./ChannelServer DEV 0
cd build/linux-debug/run/LoginServer      && ./LoginServer DEV 0
cd build/linux-debug/run/CenterServer     && ./CenterServer DEV 0
cd build/linux-debug/run/GameServer       && ./GameServer DEV 0
cd build/linux-debug/run/GlobalServer     && ./GlobalServer DEV 0
```

| Server | Port | Role |
|---|---|---|
| EPublisherServer | 8000 | login and cash |
| ChannelServer | 9400 | client's first contact: login, server and channel lists |
| LoginServer | 9200 | accounts, guilds, rankings |
| CenterServer | 9100 | field and dungeon rooms |
| GameServer | 9300 | game itself |
| GlobalServer | 9500 | global missions, auto-party, PvP, user count |

- Game-server ports come from database (`Statistics.dbo.LServerList`); EPublisherServer port from `X2_RUN_EPUBLISHER_PORT`.
- Order is not strict: GameServer retries LoginServer, CenterServer, and GlobalServer every 10 seconds. Login works only while
  EPublisherServer is running.
- Each server shows compact colored terminal log (`consolelog( n )` changes its level) and writes full log to file in its directory.
- Terminal has a **Lua prompt**, like original server window's command field: logs scroll above without breaking typed line, ↑/↓ navigate
  history, title shows `<Server>_Loading` until ready, then `_Ready`.
- **Stop:** Ctrl+C or Ctrl+D on empty line asks `Are you sure you want to exit? [y/N]`; `y` exits. SIGTERM exits without asking.
- **Without terminal** (stdin is pipe or file), each stdin line is a command and EOF exits server.
  To run as service/background, use `--no-stdin` (`./GameServer DEV 0 --no-stdin`): server waits for SIGTERM, SIGINT, or SIGHUP
  (on Windows, Ctrl+C or console close).
- **Connect to an already-running server** (service or another terminal) through `run/x2ctl`: `x2ctl list`,
  `x2ctl run GameServer 'print(1)'`, `x2ctl stop GameServer`, or `x2ctl attach GameServer` for log and JSON commands (also over `ssh`).
  Details, options (`--machine`, `--control=<name>`), and protocol in [Server console](porting/console.md).
- Executables built in container also run directly on host distribution with unixODBC and FreeTDS. Only driver path changes:
  `X2_RUN_ODBC_DRIVER` in `docker/.env`, e.g. `/usr/lib/libtdsodbc.so` on Arch.

## 5. Client

When servers are ready, open original client (`_client/`; on Linux, `wine elsword.exe`) and log in with a DB account.
Development database has account `test`, password `test`.

Working as of 2026-10-31: login, character selection, town, field, and dungeon with all six servers. Some GlobalServer features
(global missions, enchantment event, exchange limit) start without data because development DB lacks their procedures
([Original bugs](original/bugs.md#data-serverresource-clientscript)).
