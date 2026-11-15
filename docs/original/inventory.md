[Home](../README.md) › [Original code](README.md) › `_reference/X2Server` inventory

# `_reference/X2Server` inventory

`_reference/X2Server` → verified clone of `github.com/stdmaru/X2Server` (`_reference/X2Server-github`, commit `b8fb1380`;
14 commits from Jul 2026 porting to VS2026 / `v145` toolset). See below why this is no longer the HDD copy.

Solution: `ProjectX2.sln`. Line counts are raw `.c/.cpp/.h/.hpp/.inl` lines (including comments and dead code).

## In scope

| Module | Files | Lines | Role |
|---|---:|---:|---|
| `KncWX2Server/` | 1,423 | ~1.0 M | 5 servers + `Common/` (729 files) |
| `X2Lib/` | 609 | ~934 k | Client game logic |
| `KTDXLIB/` | 338 | ~216 k | Client engine (D3D9, audio, input, UI, networking) |
| `X2/` | 7 | ~3.7 k | Client `main`/executable |
| `KNCSDK_Server/` | 742 | ~226 k | Internal SDK (logging, serializer, crypto, Lua) — **most is embedded Crypto++ and Lua** |
| `KNCSDK_Client/` | 740 | ~225 k | Almost a copy of the server version (36 differing files) |
| `X2ServerProtocol/` | 38 | ~5.5 k | Client-side socket/IOCP/Actor (duplicates `Common/Socket`) |
| `LuaCommon/` | 7 | ~5.6 k | `lua_tinker` + `KLuaManager` |
| `luaLib/` | 63 | ~23 k | Lua 5.1.4 + `lua_tinker` (another copy) |
| `luajitLib/` | 147 | ~75 k | LuaJIT 2.0.2 |
| `libxml/` | 102 | ~217 k | Old libxml2 |
| `cximage/` | 433 | ~289 k | CxImage + jpeg + png |
| `Profiler/` | 18 | ~3 k | Custom profiler |
| `3rdparty/` | — | 2.6 GB | Boost 1.91 (+ precompiled libs), DXSDK, FreeType, IntelTBB 2.2 |

Relevant **project code** (excluding embedded third-party code): ~2.2 M lines before flag cleanup.

## Solution projects and dependency graph

```text
Servers (Login/Center/Channel/Game/Global)
  └─ KncWX2Server/Common ── KNCSDK_Server (lib + headers) ── boost, TBB(Game), DXSDK headers (!)
                         └─ binary libs: CrashRpt, MailMIME/SMTP, HackShield, nProtect, XTRAP, AuthCheck(Nexon), billing

X2 (exe)
  └─ X2Lib ── KTDXLIB ── KNCSDK_Client, X2ServerProtocol, libxml, CxImage, FreeType, DXSDK(d3d9/d3dx9/dinput8/dsound), FMOD
         │            └─ BandiVideo, KncFirewall, CrashRpt/BugTrap
         ├─ luajitLib / luaLib / LuaCommon
         ├─ KncWX2Server/Common  (packets and enums shared with server — single source of protocol)
         └─ NexonModule, Steamworks, IGA, XTRAP, HackShield
```

Notes:
- Servers include **DXSDK** headers (13 files use `D3DXVECTOR3`/matrices) — math only, easy to replace.
- `GameServer` includes `../../X2/dat/Script` (client data).
- `X2Lib` directly includes `KncWX2Server/Common` → `ClientPacket.h`, `CommonPacket.h`, `Enum/`, `EventID_*.h` are shared.
- `X2Lib` has 605 entries in its root directory — nearly flat organization.

## Data

- `KncWX2Server/ServerResource/*.lua` — tables read by the server.
- `DataBase/` — 1,475 `.sql` files (SSMS/DBProj project) — MSSQL schema + stored procedures.
- `ScriptData/` — `.xls/.xlsm/.mdb` source spreadsheets for tables (tool input; out of scope).
- `_client/data/` — ready-to-run client: `x2.exe`, `*.kom` (header `KOG GC TEAM MASSFILE V.0.3`, zlib contents), DLLs (d3dx9_43, fmodex, bugtrap, steam...); the effects are 4 `.fx` inside `data041.kom` ([Client](client.md#d3dx-features-that-need-replacements)).

## Copy integrity

On 2026-10-29, the original HDD copy (`/media/1tb-hdd/Storage/ElswordServer/source/X2Server`, now `_reference/X2Server-hdd`)
had **90 corrupted tracked files**: same size but content overwritten with random bytes. `git status` did not detect them because of the *stat* cache.
Git objects were intact.

Affected: `KncWX2Server` (35, including `CenterServer.vcxproj`, `.lua`/`.ini` configs and `.dsn`), `3rdparty/boost/spirit` (33),
`cximage` (21), and `KncTools` (1). No KNCSDK files were affected, so the module already imported remained valid (the report was identical with the clone).

Actions:
- `_reference/X2Server` now points to a GitHub clone at the same commit, with clean `git fsck` and all 90 files verified.
- The importer now checks the git hash of **every** file it reads ([Importer](../tools/importer.md)).
- **Recommendation**: check the HDD (SMART). Files that exist only there, outside git, may also be corrupted.

## Other references (2026-10-29)

Also under `_reference/` (read-only, outside git):

| Folder | Contents |
|---|---|
| `_original-server/` | Compiled server as it runs today: `.exe`, `config_*.ini` (CP949), `.dsn`, and `.lua` scripts (UTF-8 with BOM) it loads. Its `ServerResource` is versioned under `runtime/`; its `ClientScript` comes from another client version ([Server package](../tools/runtime.md)) |
| `AuthServer/` | Source of the DEB SOAP authentication/billing server (`_DEB_AUTH`), written by the project owner. Used for now; should be removed in a future phase |

At the root, `_database/` (outside git): SQL Server backups of the game databases (`Account`, `Game01`, `ES_BILLING`, `Statistics`),
restored by the [Development database](../tools/development-database.md).

And in `_tools/kom/`: tool to extract client `.kom` files (`_client/`), useful for inspecting scripts and assets.
Formats it reads and writes (2026-10-30, updated 2026-11-15):

| File | Entries | Lua bytecode | Status |
|---|---|---|---|
| Our client's `data036.kom` (2014) | Algorithm 2 (XORCRC) | `\x1bKL\x81` | works |
| August 2014 `data036.kom` (repository `Elsword-V5.0318.6_3`) | Algorithm 3 (XORCRC with another key, zlib without final adler32) | `\x1bKL\x82`: LuaJIT 2.0 with reordered opcodes | works |
| Brazilian August 2014 `data036.kom` (assets outside the repository, extracted by the old komextract) | Algorithm 3 with its own key (`br-2014-08`) | `\x1bKL\x82` | works (2026-11-15): all 178 encrypted entries, 139 Lua decompiled, `Item.kim` |
| Brazilian 2017 `data036.kom` (assets outside the repository), container `V.0.4` | XML header XOR a 12-byte key, a `MappedID` attribute per entry; entries Algorithm 3 with the `br-2014-08` key | `\x1bKL\x82` | works (2026-11-15): all 356 entries; 251 of 252 Lua decompiled (`DamageEffect.lua` stays bytecode: unstructured jump); its `Item.kim` (107,260 items) has 228-byte templets, which `kim.py` (188) does not read yet |
| `data036_br.kom` (2016) | Algorithm 3 in XML, but with another key or variation | ? | cannot open: entries decrypt to garbage; not retried with the regional keys or `keyrecover.py` (file not at hand on 2026-11-15) |

Algorithm 3 and the `0x82` opcode table are not in `_reference` (only 0, 1, and 2): they come from reverse engineering published in
repository `Elsword-V5.0318.6_3` (`KTDXCommonFunc.h`, `tools/kpcrack/`), performed in 2026 on a 2014 client, not KOG code. LuaJIT
version does not change in any format: all are LuaJIT 2.0.

**The Algorithm 3 key changes with the region**, while the cipher stays the same, and it did not change in Brazil from 2014
to 2017. `kom.py` keeps the known keys in `ALGO3_KEYS` and tries each one:

- **Regional table of `KOMView.exe`** (a 2023 community viewer, region table `NA EU KR BR CN TW` built in code): NA is the
  `2014-08` key above, KR is the Algorithm 2 key of our client's `KTDXCommonFunc.h`, BR decodes both Brazilian archives;
  EU, CN and TW were not checked on real archives. Its other table, about 300 five-dword tuples named by eight characters
  drawn from the `XOR_KEY` pool, matches the `MappedID` values of the `V.0.4` header (some rotated by a few characters);
  the entries decode without them, so their use is unknown.
- **`tools/kom/keyrecover.py`** finds a key from an archive's own entries when no known key fits. XORCRC is linear over
  GF(2), so every plaintext bit we can predict (zlib headers, empty Lua scripts, files also present in a decodable archive)
  is a linear equation on the 160 key bits. The equations stop at rank 144: keys form classes of 2^16 that decrypt
  identically, and the tool prints one of them. On 2026-11-15 it recovered, from the 2014 Brazilian blobs alone, a key of the
  same class as `KOMView.exe`'s BR key (and, from our client's archive, one equivalent to its Algorithm 2 key).

The `V.0.4` header key (`db4808aea7e05e588679d7af`) came from the predictable start of the XML (`<?xml version="1.0"?><Files>`).

`_reference/X2Server` (the link used by the importer) did not exist on 2026-10-29; the clone is at `_reference/X2Server-github`,
and the importer uses the clone directly if the link is missing.
