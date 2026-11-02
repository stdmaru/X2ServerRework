[Home](../README.md) › [Tools](README.md) › Development database

# Development database (SQL Server in Docker)

Local SQL Server 2022 for testing servers and `KODBC` ([ODBC database](../porting/server-common-db.md)), with game databases restored
from backups in `_database/` (outside git).

## Databases (2026-10-30)

| Backup | Database | Contents |
|---|---|---|
| `Account.bak` | `Account` | accounts (`MUser`: 5 users), 31 tables, 103 procedures |
| `Game01.bak` | `Game01` | characters, items, quests (`GUnit`, `GItem`...), 145 tables, 445 procedures |
| `ES_BILLING.bak` | `ES_BILLING` | shop/billing (`EB_Product`: 8,193 products), 40 tables, 62 procedures |
| `Statistics.bak` | `Statistics` | logs and statistics, 168 tables, 333 procedures |

Collation `Latin1_General_CI_AS`, compatibility level 100 (SQL Server 2008). Original server `.dsn` files use other database names
(`DATABASE=Elsword-Account`...): when running a server against this database, adjust `.dsn` `DATABASE` values.

## Usage

```bash
docker compose -f docker/docker-compose.yml up -d --wait     # start (creates volume first time)
docker/mssql/restore.sh                                       # restore all _database/*.bak (replaces databases)
docker compose -f docker/docker-compose.yml stop              # stop; data remains in x2-mssql-data volume
docker compose -f docker/docker-compose.yml down -v           # remove container and volume
```

- Listens only on `127.0.0.1:1433` (`X2_DB_PORT` changes port). User `sa`; default development password is in
  `docker/docker-compose.yml` and can be changed with `X2_DB_PASSWORD` (before first `up`, and also in `restore.sh`). Variables may
  be in `docker/.env` (not versioned; template `docker/.env.example`), read by `docker compose`, `restore.sh`, `docker/build.sh`,
  and CMake; shell value takes precedence over file (2026-10-31).
- The server containers also publish their client-facing ports on all host interfaces; the inter-server listeners are not published.
  This Compose file is for development, not a production deployment. See [Run the servers › Network ports](../running.md#network-ports)
  for the port list and public firewall guidance.
- `restore.sh` copies `.bak` files into container (SQL Server runs as uid 10001, which may not read mounted files), then restores
  each with `MOVE` for logical files. Accepts specific files: `docker/mssql/restore.sh _database/Game01.bak`.
- **Data adjusted for development** (`restore.sh` runs after restoring `Statistics`): `LServerList.PublicIP/PrivateIP/IP`
  and `LChannelList.IP` become `X2_SERVER_IP` (default `127.0.0.1`). Servers identify themselves in DB by address
  (`SetLocalIP` in configs), and ChannelServer sends `LChannelList` addresses to client; backup contains production addresses.
  Other IP fields are logs. Rule (2026-10-30): **data may be changed by script; schema (tables, procedures) may not**.
- Run `sqlcmd` inside the container: `docker exec -it x2-mssql /opt/mssql-tools18/bin/sqlcmd -C -S localhost -U sa -P '<password>'`.

## Connection strings

FreeTDS (`freetds` package; driver at `/usr/lib/libtdsodbc.so`, no registration in `odbcinst.ini`):

```text
DRIVER=/usr/lib/libtdsodbc.so;SERVER=127.0.0.1,1433;UID=sa;******;TDS_Version=7.4;ClientCharset=UTF-8
```

ODBC Driver 18 (`msodbcsql18`, AUR; not tested as of 2026-10-30):

```text
DRIVER={ODBC Driver 18 for SQL Server};SERVER=127.0.0.1,1433;UID=sa;******;Encrypt=yes;TrustServerCertificate=yes
```

## `KODBC` tests against database

`db` suite of `x2_server_common_tests` runs when CMake receives connection string (without `DATABASE`; tests select database):

```bash
cmake --preset linux-debug -DX2_TEST_ODBC="DRIVER=/usr/lib/libtdsodbc.so;SERVER=127.0.0.1,1433;UID=sa;******;TDS_Version=7.4;ClientCharset=UTF-8"
ctest --preset linux-debug -R server_common_db
```

Tests only read data (and create temporary procedure `#x2_test_sp`, removed with connection). Restored backups are needed:
one test reads character data for user 1 from `Game01`.
