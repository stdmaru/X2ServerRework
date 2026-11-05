[Home](../README.md) › [Tools](README.md) › Flags and verification

# Flags and verification (`gen_flags.py`, `check_equivalence.py`, `compare_flags.py`)

Three tools related to the [Importer](importer.md):
- the first discovers **which flags an original build actually defines**;
- the second proves that the `#if` resolver **does not change compiled code**;
- the third lists the flags whose value differs between two builds **in the code both compile** (shared `Common`).

```bash
python3 tools/import/gen_flags.py server            # generate flags/server.generated.toml + report
python3 tools/import/gen_flags.py client            # likewise, flags/client.generated.toml
python3 tools/import/check_equivalence.py server    # differential check across entire tree
python3 tools/import/check_equivalence.py server --show Common/BaseServer.cpp   # diff one file
python3 tools/import/compare_flags.py server client --also X2Lib/X2UDPPacket.h X2Lib/X2UDPPacket.cpp \
    --report reports/flags-shared/REPORT.md
python3 tools/import/tests/test_flagsim.py
```

## Profiles

A profile (`tools/import/flags/<profile>.profile.toml`, read by `x2import/profile.py`) describes an original build:

| Key | Meaning |
|---|---|
| `root` | folder under `_reference/X2Server` that the other paths are relative to |
| `entry` / `entries` | configuration header(s) that a translation unit reads, **in order**, sharing one macro table |
| `include_dirs` | where quoted includes are searched |
| `defines` | vcxproj definitions of the service build |
| `dev_defines`, `dev_undefines` | changes for the development build (add `_IN_HOUSE_`; the client also drops `_SERVICE_`) |
| `config_globs` | configuration headers: every name they `#define`/`#undef` (even commented out or for another country) is a flag; the simulation only enters these files |
| `code_globs`, `code_exclude_globs` | code scanned for flags used in conditionals and for macros defined in ordinary code |
| `not_flags` | platform/SDK or utility macros that a configuration header defines (`WINVER`, `TRACE`...) |
| `flag_prefixes` | names used in code but defined nowhere become `false` only with these prefixes |
| `[[profile.layers]]` | part of the build that reads a different chain (`name`, `entries`, `code_globs`); reported and checked separately |

## `gen_flags.py <profile>`

1. **Simulate** preprocessor on configuration headers, following `#include "..."` into other configuration headers only, in a
   closed world: anything undefined is false. Run twice, service and dev (`x2import/flagsim.py`).
2. **Flag universe**: every name appearing in `#define`/`#undef` in configuration headers for *all* countries, including commented-out (`//#define SERV_X`),
   plus every flag-prefixed (`SERV_`) name used in code `#if`.
3. Remove from universe:
   - function-like macros;
   - names handled manually (`base.toml`);
   - build defines;
   - names also defined in common code, which remain unresolved and are reported.
4. Write `flags/<profile>.generated.toml` (`true` = defined in service build, `false` = otherwise) and
   `reports/flags-<profile>/REPORT.md`, containing:
   - flags that exist only in dev (whitelisted ones become `X2_DEV_TOOLS`);
   - flags with values;
   - locally defined flags;
   - flags used in code but not mentioned in configuration.

   - for each layer, flags tested in its code whose value in its own chain differs from the profile's.
4. Importer module load order: `flags = ["base", "server.generated", "server"]`. **Later files take precedence.**
`server.toml` is maintained by hand and contains server-specific exceptions (below).

## `check_equivalence.py <profile>`

For each tree file:
1. Fully preprocess the **original** under its actual configuration (simulation result + exceptions), respecting local `#define`/`#undef`.
2. Do the same for the importer-resolved file.
3. Surviving code lines must be identical.

Intentional removals excluded from comparison: flag `#define`/`#undef`, `#pragma comment(lib)`, and blank lines.

Repeat for service and dev configurations, each with two scenarios: unknown macros (`_WIN32`, guards...) undefined, and all defined.
Files of a layer are compared with that layer's own chain: names whose value there differs from the profile keep the
layer's original value, so the check shows what the single generated set changes in that layer.

## `compare_flags.py <a> <b>`

Takes the code files both profiles scan (configuration headers excluded) and lists each flag tested there whose state
(`true`, `false`, `X2_DEV_TOOLS`, unknown) differs between `base + <a>.generated + <a>` and `base + <b>.generated + <b>`,
with file and line, and whether the file defines packets (`#pragma pack` or `*Packet*`).

## Server results (2026-10-29)

- `server.generated.toml`: **736 true flags, 423 false**. Visited headers: `ServerDefine.h` → `ServerDefine_Global.h` → `ServerDefine_US.h`.
- Dev-only: `DELETE_INCORRECT_ERROR_LOG` and `SERV_LOG_UNDEFINED_QUEST_TEMPLET`, both logging flags included in dev whitelist.
- Verification: **1,415 of 1,423 files equivalent**.
  The remaining 8 are configuration headers. `ServerDefine.h` enables `_IN_HOUSE_` in `_DEBUG` builds; now that role belongs to `X2_DEV_TOOLS`.
  Other-country headers test flags they define themselves and are never compiled in US.

## Server exceptions (`flags/server.toml`)

| Flag | Value | Reason |
|---|---|---|
| `_SERVICE_` | false | Server `Release_US` vcxproj files do **not** define it (only client does). In server, `#ifndef _SERVICE_` was active code |
| `_DEB_AUTH` | true | DEB ("Debelzak Custom Auth", project-owner customization with custom SOAP server): authentication (Channel) and billing (Game) used here, replacing US publisher. Defined only in Channel and Game; all files testing it are compiled only in those two |

## Client results (2026-11-05)

The client build is `US_INTERNAL|Win32` (the solution's `Release_US`), the same definitions in `X2`, `X2Lib` and
`KTDXLIB`: `WIN32 NDEBUG _WINDOWS SERV_COUNTRY_US CLIENT_COUNTRY_US _SERVICE_` (plus non-flag build macros). Despite its
name it is a service build; the other countries' `*_INTERNAL` builds replace `_SERVICE_` with `_IN_HOUSE_`, which is the
client's development build here.

- **Two chains.** Every `X2Lib`/`X2` file reads `ServerDefine.h` **before** `KTDX.h`: `stdafx.h` → `X2Main.h` →
  `X2ServerProtocolLib.h` → `SystemPacket.h` → `CommonPacket.h` → `ServerDefine.h`, then `KTDX.h` → `Always.h` →
  `OnlyGlobal/Always_Global.h` + `Always_US.h` → `AlwaysButConditionally.h`. `KTDXLIB` files read only `KTDX.h`
  (`stdafx.h`), so it is a layer of the profile. `X2ServerProtocol` builds as `X2TOOL|Win32` in `Release_US` (no
  `_SERVICE_`, no country, and `ServerDefine.h` turns on `_IN_HOUSE_` under `X2TOOL`); its own code tests no feature flag,
  only the packet headers it includes. The port compiles it with the client set.
- `client.generated.toml`: **1,719 true, 890 false**. Visited headers: `ServerDefine.h` → `ServerDefine_Global.h` →
  `ServerDefine_US.h` → `KTDX.h` → `Always.h` → `Always_Global.h` → `Always_US.h` → `AlwaysButConditionally.h`.
  Full lists: [client report](../../tools/import/reports/flags-client/REPORT.md).
- Dev-only: 74 names, 28 of them already in the `X2_DEV_TOOLS` whitelist (`base.toml`); the other 46 (unreleased
  gameplay changes and some QA helpers, such as `ALWAYS_SCREEN_SHOT_TEST` or `VILLAGE_MAP_FAST_RELOADING`) resolve as in
  the service build. Whitelisting more is a later triage ([Debug and development](../original/debug-and-development.md)).
- `_DUMP_ITEM_DATA_` (`X2Lib/Debelzak/DumpData.cpp`, the project owner's item/quest data export) is commented out in
  `Always.h` and every call to it is commented out too, so it resolves to false and the file is not imported.
- Engine layer: one flag changes there, `SERV_TRAPPING_RANGER_TEST` (defined by `ServerDefine.h`, so false in the original
  `KTDXLIB`). It adds `SRO_VITALPOINT_PIERCING` to an enum in `KTDXRandomNumbers.h` that the engine's own `.cpp` does not
  use; `X2Lib`, which uses it, already saw the value true. One set for every layer removes that ODR mismatch.
- Verification: **973 of 978 files equivalent** (688/688 game and shared files, 285/290 engine files). The other 5 are
  `KTDXRandomNumbers.h` (above) and the `Always_BR/JP/PH/TWHK.h` headers, which test flags they define themselves and
  are never compiled in US.

## Server and client in shared code (2026-11-05)

Code compiled by both programs (48 files): the 46 files of the server's `Common` that the client includes (packets, event
IDs, enums, `KncUidType.h`, `ScriptID_Code.h`, `NetError`), all of them already in `src/server/Common` except the
EU-only `NetError_def_<language>.h` (`UNITED_CLIENT_EU`, false), plus the client's `X2Lib/X2UDPPacket.*` that the servers
compile (`server-x2lib`, in `src/server/X2Lib`).
[`compare_flags.py`](../../tools/import/reports/flags-shared/REPORT.md) finds **two** flags with different states there:

| Flag | Server | Client | Where | Effect | Handling |
|---|---|---|---|---|---|
| `ADD_RENA_SYSTEM` | unknown (never defined; the server import kept the `#ifdef`) | false (commented out in `Always.h`) | `CommonPacket.h`: an enum value and an accessor of `KGamePlayStatus` | none: undefined in both builds | nothing; the dead `#ifdef` can go in Phase 5 |
| `X2OPTIMIZE_NPC_NONHOST_SIMULATION` | unknown (left as `#ifdef`, so undefined) | true (`KTDX.h`) | `X2UDPPacket.*`: extra fields and the serialization of `KDYNAMIC_UNIT_NPC_SYNC`, and `KDYNAMIC_UNIT_NPC_MINISYNC` | the original servers and client had different layouts, harmless because the servers never use these structs (they relay P2P datagrams; `X2Packet.cpp` uses the port-check structs) | at the client import, resolve it as **true** by hand in `src/server/X2Lib` (that module is hand-edited, so it is not reimported), which is the layout on the wire |

Across the two generated sets, the 1,188 names they share have the same value; the only environment difference is
`_SERVICE_`, which no shared file tests. The shared files are therefore kept once and compiled by both (decision
[T33](../project/decisions.md#accepted)).
