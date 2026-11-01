[Home](../README.md) › [Tools](README.md) › Flags and verification

# Flags and verification (`gen_flags.py`, `check_equivalence.py`)

Two tools related to the [Importer](importer.md):
- the first discovers **which flags an original build actually defines**;
- the second proves that the `#if` resolver **does not change compiled code**.

```bash
python3 tools/import/gen_flags.py server            # generate flags/server.generated.toml + report
python3 tools/import/check_equivalence.py server    # differential check across entire tree
python3 tools/import/check_equivalence.py server --show Common/BaseServer.cpp   # diff one file
python3 tools/import/tests/test_flagsim.py
```

## `gen_flags.py <profile>`

The profile (`tools/import/flags/<profile>.profile.toml`) describes the original build:
- input header;
- vcxproj defines;
- extra defines for a dev build (`_IN_HOUSE_`);
- which files are configuration headers.

1. **Simulate** preprocessor on configuration headers, following `#include "..."` in a closed world: anything undefined is false.
   Run twice, service and dev (`x2import/flagsim.py`).
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

Importer module load order: `flags = ["base", "server.generated", "server"]`. **Later files take precedence.**
`server.toml` is maintained by hand and contains server-specific exceptions (below).

## `check_equivalence.py <profile>`

For each tree file:
1. Fully preprocess the **original** under its actual configuration (simulation result + exceptions), respecting local `#define`/`#undef`.
2. Do the same for the importer-resolved file.
3. Surviving code lines must be identical.

Intentional removals excluded from comparison: flag `#define`/`#undef`, `#pragma comment(lib)`, and blank lines.

Repeat for service and dev configurations, each with two scenarios: unknown macros (`_WIN32`, guards...) undefined, and all defined.

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
