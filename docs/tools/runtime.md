[Home](../README.md) › [Tools](README.md) › Run directories (`runtime/`)

# Run directories (`runtime/`, `x2_run_*`)

Decision T22 in [Decisions](../project/decisions.md). Created on 2026-10-30 for `server/channel-run`. Nothing here depends on
`_reference`, which will be deleted: everything the servers read is either versioned under `runtime/` or generated from the client
(`_client/`).

## Usage

```bash
docker compose -f docker/docker-compose.yml up -d --wait && docker/mssql/restore.sh   # database (see Development database)
cmake --build --preset linux-debug                                                   # build and assemble all run directories
cmake --build --preset linux-debug --target x2_run_channel                           # or only one
cd build/linux-debug/run/ChannelServer && ./ChannelServer DEV 0                      # tag DEV, server group 0
cmake --build --preset linux-debug --target x2_run_game                              # GameServer (port 9300)
cd build/linux-debug/run/GameServer && ./GameServer DEV 0
cmake --build --preset linux-debug --target x2_run_login                             # LoginServer (port 9200)
cd build/linux-debug/run/LoginServer && ./LoginServer DEV 0
cmake --build --preset linux-debug --target x2_run_center                            # CenterServer (port 9100)
cd build/linux-debug/run/CenterServer && ./CenterServer DEV 0
cmake --build --preset linux-debug --target x2_run_global                            # GlobalServer (port 9500)
cd build/linux-debug/run/GlobalServer && ./GlobalServer DEV 0
```

Login goes through EPublisherServer ([Server: EPublisherServer](../porting/server-epublisher.md)), which must be running:

```bash
cmake --build --preset linux-debug --target x2_run_epublisher
cd build/linux-debug/run/EPublisherServer && ./EPublisherServer DEV     # port 8000 (X2_RUN_EPUBLISHER_PORT)
```

Since 2026-10-31, normal builds assemble all server directories (`X2_RUN_FOLDERS`, ON by default, adds `x2_run_*` to `ALL`) in all
presets. Incremental builds use a stamp to regenerate `ClientScript` only when the client changes, and copy files only when needed;
a no-op build takes less than a second. Without the client (`X2_CLIENT_DATA`), `ClientScript` is skipped with a warning. CI configures
with `-DX2_RUN_FOLDERS=OFF` (no client or database).

These are **development environments** (with a local database, an ODBC driver, and the client from the build machine), not deployment
packages. Creating a release package with configuration templates is a separate step (`cmake --install`, not implemented yet).

## Assembled layout

| Directory | Source |
|---|---|
| `build/<preset>/run/ServerResource/` | copy of `runtime/ServerResource/` |
| `build/<preset>/run/ClientScript/` | generated from `X2_CLIENT_DATA` `.kom` by `tools/runtime/gen_clientscript.py` |
| `build/<preset>/run/<Server>/` | executable, files from `runtime/<Server>/`, filled `*.in` templates; empty `Statistics/` |

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
| `X2_RUN_DIR` | `build/<preset>/run` | destination |
| `X2_CLIENT_DATA` | `_client/data` | `ClientScript` source |
| `X2_RUN_DB_SERVER`, `X2_RUN_DB_USER`, `X2_RUN_DB_PASSWORD` | `127.0.0.1,<X2_DB_PORT or 1433>`, `sa`, development password from `docker-compose.yml` | `.dsn` |
| `X2_RUN_DB_NAME_PREFIX` | empty | Prefix for database names in `.dsn` files. Set to `Elsword-` if the restored databases are named `Elsword-Account`, `Elsword-Statistics`, etc. (2026-10-31) |
| `X2_RUN_ODBC_DRIVER`, `X2_RUN_ODBC_OPTIONS` | FreeTDS (`libtdsodbc.so`, found by `find_library` in `/usr/lib` on Arch or `/usr/lib/<triplet>/odbc` on Ubuntu; `TDS_Version=7.4;ClientCharset=UTF-8`) if available, else ODBC Driver 18 (`Encrypt=yes;TrustServerCertificate=yes`) | `DBLayer:SetODBCDriver/SetODBCOptions` |
| `X2_RUN_SERVER_IP` | `127.0.0.1` | `SetLocalIP` (the address the server registers in the database) |
| `X2_RUN_EPUBLISHER_PORT`, `X2_RUN_EPUBLISHER_URL` | `8000`, `http://127.0.0.1:8000/` | EPublisherServer port; server `DEBAuthManager:AddAuthServiceAddress` |

Password exists only in cache and generated files under `build/`, never in git.

**Files under `run/<Server>/` are regenerated on every build** (`copy_if_different`): hand edits to `.dsn` files or configs there are
replaced by the template on the next build (as happened on the Windows VM on 2026-10-31). To use another database or address, change
the variables above and reconfigure.
On Windows, `ClientScript` is not generated (requires `luajit` and client): copy it from a Linux build to `run/ClientScript`.

**Environment variables and `docker/.env`** (2026-10-31): each variable in the table can also be set through an environment variable
with the same name, which **overrides the cache on every configure** (`x2_env_cache` in `cmake/X2Runtime.cmake`); otherwise, the
cache value (`-D` or default) is used.
Three names are also read by `docker compose` and `restore.sh`: password from `X2_DB_PASSWORD`, address from `X2_SERVER_IP`, and
`X2_RUN_DB_SERVER` port from `X2_DB_PORT`. Empty values are ignored; relative paths are resolved from repository root.
`docker/build.sh` passes all `X2_*` variables from the environment and `docker/.env` (not versioned; template with all variables in
`docker/.env.example`) into the container. Changes take effect on the next configure (`cmake --preset <preset>`) and are then cached.

**Run on the host** (for testing, without Docker): the binary built in the image runs directly on a distribution with the same
libraries (unixODBC, FreeTDS); only the driver path changes. Configure with `-DX2_RUN_ODBC_DRIVER=/usr/lib/libtdsodbc.so`
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
