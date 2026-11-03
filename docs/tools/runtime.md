[Home](../README.md) › [Tools](README.md) › Server package (`dist/`)

# Server package (`dist/`, `x2_run_*`)

Decision T22 in [Decisions](../project/decisions.md). Created on 2026-10-30 for `server/channel-run`. Nothing here depends on
`_reference`, which will be deleted: everything the servers read is either versioned under `runtime/` or generated from the client
(`_client/`).

## Usage

```bash
docker compose -f docker/docker-compose.yml up -d --wait && docker/mssql/restore.sh   # database (see Development database)
cmake --build --preset linux-debug                                                   # build and assemble dist/
cmake --build --preset linux-debug --target x2_run_channel                           # or only one
cd dist/ChannelServer && ./ChannelServer DEV 0                      # tag DEV, server group 0
cmake --build --preset linux-debug --target x2_run_game                              # GameServer (port 9300)
cd dist/GameServer && ./GameServer DEV 0
cmake --build --preset linux-debug --target x2_run_login                             # LoginServer (port 9200)
cd dist/LoginServer && ./LoginServer DEV 0
cmake --build --preset linux-debug --target x2_run_center                            # CenterServer (port 9100)
cd dist/CenterServer && ./CenterServer DEV 0
cmake --build --preset linux-debug --target x2_run_global                            # GlobalServer (port 9500)
cd dist/GlobalServer && ./GlobalServer DEV 0
```

Login goes through EPublisherServer ([Server: EPublisherServer](../porting/server-epublisher.md)), which must be running:

```bash
cmake --build --preset linux-debug --target x2_run_epublisher
cd dist/EPublisherServer && ./EPublisherServer DEV     # port 8000 (X2_EPUBLISHER_PORT)
```

Normal builds assemble all server directories under repository-root `dist/` (`X2_DIST_FOLDERS`, ON by default, adds `x2_run_*` to
`ALL`) for every preset. The destination is shared by presets. Incremental builds use a stamp to regenerate `ClientScript` only when
the client changes, and copy files only when needed. Without the client (`X2_CLIENT_DATA`), `ClientScript` is skipped with a warning.
CI configures with `-DX2_DIST_FOLDERS=OFF` (no client or database).

`dist/` is the assembled package for running and testing the servers. CMake writes the selected database settings, local ODBC driver,
and client paths into its configs and `.dsn` files during configuration/build.

## Assembled layout

| Directory | Source |
|---|---|
| `dist/ServerResource/` | copy of `runtime/ServerResource/` |
| `dist/ClientScript/` | generated from `X2_CLIENT_DATA` `.kom` by `tools/runtime/gen_clientscript.py` |
| `dist/<Server>/` | executable, files from `runtime/<Server>/`, filled `*.in` templates; empty `Statistics/` |

Configs point to `../ServerResource` and `../ClientScript` (`SimLayer:AddPath`), as original pointed to `C:\Elsword\...`.

## Versioned under `runtime/`

- `ServerResource/`: 84 production server scripts (`_original-server/ServerResource`, 20 MB), with their bytes intact
  (`-text` in `.gitattributes`: mixed CRLF/LF and BOM, as found), **with each server-folder version replacing the corresponding
  `ServerResource` version** (see below, T28). On 2026-10-30, the user chose these files over those in the `X2Server` repository
  (85 MB,
  country folders and KOG values: production server has max level 67, custom ad URL, and extra files).
- `<Server>/`: no data `.lua` files (configs only); original files without secrets (`config_ch.ini`) and `*.in` templates:
  `config_<x>_DEV.lua.in` (derived from `config_*_US_SERVICE.lua`, comments translated) and one `.dsn.in` per database.

### Template values (CMake cache)

| Variable | Default | Used in |
|---|---|---|
| `X2_DIST_DIR` | `<repository>/dist` | package destination, shared by presets |
| `X2_CLIENT_DATA` | `_client/data` | `ClientScript` source |
| `X2_DB_SERVER`, `X2_DB_USER`, `X2_DB_PASSWORD` | `127.0.0.1,<X2_DB_PORT or 1433>`, `sa`, development password from `docker-compose.yml` | `.dsn` |
| `X2_ODBC_DRIVER`, `X2_ODBC_OPTIONS` | FreeTDS (`libtdsodbc.so`, found by `find_library` in `/usr/lib` on Arch or `/usr/lib/<triplet>/odbc` on Ubuntu; `TDS_Version=7.4;ClientCharset=UTF-8`) if available, else ODBC Driver 18 (`Encrypt=yes;TrustServerCertificate=yes`) | `DBLayer:SetODBCDriver/SetODBCOptions` |
| `X2_SERVER_IP` | `127.0.0.1` | `SetLocalIP` (the address the server registers in the database) |
| `X2_EPUBLISHER_PORT`, `X2_EPUBLISHER_URL` | `8000`, `http://127.0.0.1:8000/` | EPublisherServer port; server `DEBAuthManager:AddAuthServiceAddress` |
| `X2_SERVERRESOURCE_DIR`, `X2_CLIENTSCRIPT_DIR` | `../ServerResource`, `../ClientScript` | Resource search paths baked into config templates; server processes can override them from their environment at runtime |
| `X2_GAMESERVER_PORT` | `9300` | GameServer client-facing port read from process environment at runtime; Docker Compose sets it per GameServer instance |

The database password is written to the CMake cache and generated `.dsn` files under `dist/`; both locations are ignored by git.

**Files under `dist/<Server>/` are regenerated on every build** (`copy_if_different`): hand edits to `.dsn` files or configs there are
replaced by the template on the next build (as happened on the Windows VM on 2026-10-31). To use another database or address, change
the variables above and reconfigure.
On Windows, `ClientScript` is not generated (requires `luajit` and client): copy it from a Linux build to `dist/ClientScript`.

**One `docker/.env` for build and runtime:** `docker/build.sh` loads this file and passes its `X2_*` variables to CMake, where
non-empty environment values override the preset cache at configure time (`x2_env_cache` in `cmake/X2Runtime.cmake`). The generated
configs and `.dsn` files then keep those values in `dist/`. The development server Compose file
(`docker/server/docker-compose.yml`) also loads `docker/.env` into each server container at startup. Runtime environment lookups,
including resource path overrides, can use those values. `X2_GAMESERVER_PORT` is assigned per instance in the Compose file, whose
service-level `environment` entries override values from `env_file`. A direct host launch does not load `.env` automatically; export
its values in the shell first.
Docker Compose and `restore.sh` also use `X2_DB_PASSWORD`, `X2_DB_PORT`, and `X2_SERVER_IP` for the development database.
Empty values are ignored by CMake; relative paths are resolved from repository root. See [`.env.example`](../../docker/.env.example).

**Run on the host** (for testing, without Docker): the binary built in the image runs directly on a distribution with the same
libraries (unixODBC, FreeTDS); only the driver path changes. Configure with `-DX2_ODBC_DRIVER=/usr/lib/libtdsodbc.so`
(Arch, 2026-10-31) and assemble the directories again (`x2_run_*`). The host's `gdb` provides a crash backtrace:
`gdb -batch -ex run -ex bt --args ./GameServer DEV 0`.

## `ClientScript` generated from the client

`tools/runtime/gen_clientscript.py <data> <output>`: decrypts each `.lua` in `.kom` and decompiles KOG bytecode (`tools/kom/kom.py`),
and transforms `Item.kim` into `Item.lua` + `SetItem.lua` (`tools/kom/kim.py`, which uses LuaJIT to read enums: the system-installed
version or, when unavailable as in the build image, the interpreter built by CMake from the commit pinned in T21).
Scripts are flattened into one output directory (`KAutoPath` searches subfolders by name, and no names repeat across `.kom` files).
Measured on 2026-10-30 with the client in `_client/`: 4,564 scripts from 173 `.kom`, 45 seconds. The output stamp avoids reruns
when nothing changes.

**Comparison with original server's `ClientScript`** (2026-10-30, behavioral comparison using `tools/runtime/lua_trace.lua`, which
records calls, arguments, and defined globals): 2,198 scripts behave equivalently (2 are byte-identical), 1,800 differ, and 68 are
absent from the client.

- **Version**: original server's `ClientScript` comes from newer client (`DungeonEnum.lua` has dungeons from 2016–2019).
  Dungeon IDs used by `ServerResource` exist in both versions.
- **Format**: the server copy predates KOG's preprocessing step (`X2ScriptPreprocessingTool`): vectors are text inside tables
  (`"START_POS = D3DXVECTOR3(0, 19200, 0)"`); client has calls (`START_POS = D3DXVECTOR3(0, 19200, 0)`). Appears in line maps
  (`World/`) and dialogs. **Verify in GameServer**, which reads these files (`lua_tinker::get<D3DXVECTOR3>`).
- **Absent from the client**: event scripts (`*_Festival`, `Halloween_2013`, `Mission_Impossible`...), translation (`*Trans.lua`),
  and `NewSkillTemplet.lua`.

ChannelServer reads only `Enum.lua` and `DungeonEnum.lua` from `ClientScript`. GameServer reads almost everything; files too large for
LuaJIT go through `lua_dofile` ([KNCSDK](../porting/kncsdk.md)), and production-data load errors are in
[Original bugs](../original/bugs.md#data-serverresource-clientscript).

**GameServer** (`runtime/GameServer`, 2026-10-31): `config_gs_DEV.lua.in` derived from production `config_gs_US_SERVICE.lua`
(already had no `CF_XTRAP` or HackShield), with FTP statistics disabled; `.dsn` for `Account`, `Game01`, `Statistics`, and
`ES_BILLING` (production had `Game01`/`Game02` per group, both in the same database); `config_gs.ini` unchanged (version `V4.0326.5`).
Loose `.lua` files in production GameServer folder **were loaded** and moved to `ServerResource` (below).

**LoginServer** (`runtime/LoginServer`, 2026-10-31): `config_lg_DEV.lua.in` derived from `config_lg_US_SERVICE.lua` (Nexon already
commented out), `.dsn` for `Account`, `Game01`, `Statistics`, and `ES_BILLING`; `config_lg.ini` unchanged. Production LoginServer
folder's `RankingData.lua` (Henir rewards through rank 6, vs. 3 in `ServerResource`) was loaded and moved to `ServerResource`.
`RefreshTimeBackup.ini` (next reset time for each Henir ranking) is created by server at shutdown in its own folder, not versioned.

**CenterServer** (`runtime/CenterServer`, 2026-10-31): `config_cn_DEV.lua.in` derived from `config_cn_US_SERVICE.lua` (SMS already
off), `.dsn` for `Account`, `Game01`, `Statistics`, and `config_cn.ini` unchanged. Production `Config/` (other-country configs) omitted.

**GlobalServer** (`runtime/GlobalServer`, 2026-10-31): `config_gb_DEV.lua.in` derived from `config_gb_US_SERVICE.lua` (SMS and NXWeb
already commented), `.dsn` for `Account` and `Statistics` (two DBs in US config), and `config_gb.ini` unchanged.
The production folder's `GlobalMissionConfig.lua` and `MissEliosConfig.lua` are not loaded by anything (T28).

## Scripts from each server folder (2026-10-31, T28)

`KAutoPath` (`Common/KAutoPath.cpp`) searches for a script **first in current directory** and its subfolders, then in `AddPath`s.
In production each service ran with its own folder as current directory (`nssm set ... AppDirectory C:\Elsword\<Server>\`,
`_original-server/Install-Services.ps1`), so loose `.lua` files there **took precedence over** those in `ServerResource`. Port retains
search order; all scripts are stored in `ServerResource`, using the server-folder version, to reproduce production behavior.

Comparison from 2026-10-31 (server folder × `ServerResource` × `KncWX2Server/ServerResource/US` from the `X2Server` repository as a baseline):

| Status | Files | Action |
|---|---|---|
| Differ from `ServerResource` | GameServer: `CashItemPrice`, `ClassChangeItem`, `ClassChangeMemo`, `ClassChangeQuest`, `ClassChangeSkill`, `ClassChangeTable`, `FieldData`, `ManufactureResultTable`, `MapData`, `PetData`, `RandomItemData`, `RandomItemMapping`, `RandomItemTable`, `RecommendUserTable`, `StatTable`; CenterServer: `DropTable`, `PaymentItemTable`, `SecretDungeonData`, `StaticDropTable`; LoginServer: `RankingData`; GlobalServer: `WorldMissionConfig` (line endings only) | server-folder version replaces `ServerResource` version (bytes preserved) |
| Match `ServerResource` | remaining 11 GameServer files and 13 CenterServer files | no action |
| Only in server folder and unused | `BoardBox`, `BoardBox2`, `CardTable`, `GlobalMissionConfig`, `MissEliosConfig` (GameServer and GlobalServer versions differ) | Not included: no code, config, or script references these names. Revisit GlobalServer `MissEliosConfig` when ported |

Server-folder version is not always newer: in 9 files, `ServerResource` was untouched baseline and only server folder had edits
(`CashItemPrice` from 2012-02 vs. 2011-06), but for `StatTable` (Ara classes, 2013-08 vs. 2012-05), `RandomItemData`,
`RandomItemMapping` (events through 2014-08), and `PaymentItemTable`, `ServerResource` was newer and **never loaded** in production.
Replaced versions remain in git history (initial `runtime/ServerResource` commit) and `X2Server` repository.

## `tools/kom`

`kom.py` (`list`, `unpack`, `pack`, `decompile`) and `kim.py` (`decompile`, `compile`, `info`), brought from `_tools/kom` on
2026-10-30. Formats in [Inventory](../original/inventory.md). To **pack** (`pack`/`compile`), tools need KOG's LuaJIT 2.0.2
(`luajit-kog/luajit` beside script, or `$KOM_LUAJIT`/`--luajit`), currently built from `_reference` sources: pending moving this
recipe into repository before `_reference` is removed. Extracting and decompiling do not need it.
