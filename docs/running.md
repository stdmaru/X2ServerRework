[Home](README.md) › Run the servers

# Run the servers

How to start the server suite and connect with the original client. The steps below are for Linux, the only platform on which the
servers had been run as of 2026-10-31. Build first: [Linux](building-linux.md) or [Windows](building-windows.md).

Commands prefixed with `docker/build.sh` run in build container. For native build, run same command without prefix.

## Requirements

- **Database backups** in `_database/` (`Account.bak`, `Game01.bak`, `Statistics.bak`, `ES_BILLING.bak`).
- **Original client** in `_client/`: servers read its scripts (generated from `.kom`), and it is used to play.
  On Linux, the client runs through **Wine**.
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

The defaults work when everything runs on the same machine. To change a setting (such as the password, database port, advertised
server address, ODBC driver, or client directory), copy the template and edit the relevant values:

```bash
cp docker/.env.example docker/.env
```

`docker/.env` is not versioned. `docker/build.sh` loads it for CMake configuration and package assembly; the server Compose file
(`docker/server/docker-compose.yml`) loads it into server containers when they start. Database Compose and `restore.sh` use its DB
settings too. Values used to generate configs and `.dsn` files are baked into `dist/` at configure/build time; process environment
values are read at runtime only where a server explicitly checks them. When starting servers directly on the host, load the file into
the shell first (`set -a; source docker/.env; set +a`). Shell variables override file values, and Compose service-level environment
settings override `env_file`. [`docker/.env.example`](../docker/.env.example) describes the variables and when they apply.

## 3. Server package

Build assembles the server package under repository-root `dist/`:

```text
dist/
├── ServerResource/      server scripts (from repository)
├── ClientScript/        scripts generated from the client
├── EPublisherServer/    executable + config_*_DEV.lua + .dsn (already pointed at DB)
├── ChannelServer/
├── LoginServer/
├── CenterServer/
├── GameServer/
├── GlobalServer/
└── x2ctl
```

To assemble only one: `docker/build.sh cmake --build --preset linux-debug --target x2_run_game`.
The package contains the configured server folders and shared scripts. See [Server package](tools/runtime.md) for details.

## 4. Start servers

Start each server in its own directory, using a separate terminal for each. The first argument selects the configuration
(`DEV` reads `config_*_DEV.lua`); the second is the server group:

Or start them all, in order, from [ServerManager](tools/servermanager.md#window) (`ServerManager dist`), which also shows their logs
and a Lua prompt for each.

```bash
cd dist/EPublisherServer && ./EPublisherServer DEV
cd dist/ChannelServer    && ./ChannelServer DEV 0
cd dist/LoginServer      && ./LoginServer DEV 0
cd dist/CenterServer     && ./CenterServer DEV 0
cd dist/GameServer       && ./GameServer DEV 0
cd dist/GlobalServer     && ./GlobalServer DEV 0
```

### Network ports

The client does **not** connect to every server listener. The services use internal connections to each other; only the client-facing
listeners (including relay traffic) need to be reachable from players:

| Port/protocol | Service | Use |
|---|---|---|
| `9400/TCP` | ChannelServer | Client's first contact: login, server and channel lists |
| `9300/TCP` | GameServer | Main game connection |
| `9301/TCP` | GameServer | TCP relay |
| `9301/UDP` | GameServer | UDP game traffic |
| `9101/UDP` | CenterServer | UDP relay for rooms and fields |

These are the public mappings in [`docker-compose.yml`](../docker/docker-compose.yml). The Compose `ports` entries without a host
address publish on all host interfaces, so restrict the public firewall/security-group rules to the table above. The SQL mapping is
bound only to `127.0.0.1` and is not public.

The other listeners are for server-to-server communication and should **not** be exposed to players or the public Internet:
EPublisherServer `8000/TCP`, LoginServer `9200/TCP`, CenterServer `9100/TCP`, and GlobalServer `9500/TCP`. In Compose they are
reachable by service name over the private `x2-net` network and have no published host ports.

Game-server TCP/UDP ports come from `Statistics.dbo.LServerList` (and the addresses sent by ChannelServer come from
`Statistics.dbo.LChannelList`); the Compose mappings must match those database values. EPublisherServer's port comes from
`X2_EPUBLISHER_PORT`, and its URL is used by the other servers. For clients outside the host, configure the advertised server
address as a reachable public IP (`X2_SERVER_IP`/`X2_SERVER_IP`), then regenerate the server configuration and restore/update the
database addresses. The default `127.0.0.1` is for local development and will not work for remote clients.

- Startup order is flexible: GameServer retries LoginServer, CenterServer, and GlobalServer every 10 seconds. Login works only while
  EPublisherServer is running.
- Each server shows compact colored terminal log (`consolelog( n )` changes its level) and writes full log to file in its directory.
- Terminal has a **Lua prompt**, like original server window's command field: logs scroll above without breaking typed line, ↑/↓ navigate
  history, title shows `<Server>_Loading` until ready, then `_Ready`.
- **Stop:** Ctrl+C or Ctrl+D on empty line asks `Are you sure you want to exit? [y/N]`; `y` exits. SIGTERM exits without asking.
- **Without a terminal** (stdin is a pipe or file), each line is treated as a command, and EOF causes the server to exit.
  To run as service/background, use `--no-stdin` (`./GameServer DEV 0 --no-stdin`): server waits for SIGTERM, SIGINT, or SIGHUP
  (on Windows, Ctrl+C or console close).
- **Connect to an already-running server** (service or another terminal) through `dist/x2ctl`: `x2ctl list`,
  `x2ctl run GameServer 'print(1)'`, `x2ctl stop GameServer`, or `x2ctl attach GameServer` for log and JSON commands (also over `ssh`).
  Details, options (`--machine`, `--control=<name>`), and protocol in [Server console](porting/console.md).
- Executables built in container also run directly on host distribution with unixODBC and FreeTDS. Only driver path changes:
  `X2_ODBC_DRIVER` in `docker/.env`, e.g. `/usr/lib/libtdsodbc.so` on Arch.

## 5. Client

When servers are ready, open original client (`_client/`; on Linux, `wine x2.exe`) and log in with a DB account.
Development database has account `test`, password `test`.

As of 2026-10-31, login, character selection, the town, a field, and a dungeon work with all six servers. Some GlobalServer features
(global missions, enchantment event, exchange limit) start without data because development DB lacks their procedures
([Original bugs](original/bugs.md#data-serverresource-clientscript)).
