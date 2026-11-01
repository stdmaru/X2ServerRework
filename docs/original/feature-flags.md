[Home](../README.md) › [Original code](README.md) › Feature flags and variants

# Feature flags and variants

## Scale of the problem

- ~**1,175** distinct `SERV_*` macros across server + client.
- ~**58,600** `#if/#ifdef/#ifndef` lines in `KncWX2Server` + `X2Lib` alone.
- `#ifdef SERV_*` appears in 874 server files and 324 X2Lib files.
- `KncWX2Server/Common/ServerDefine.h` has **4,645 lines**, ~700 `#define`s (133 commented out), each with a Korean comment block
  (author, date, description). Client equivalents: `KTDXLIB/Always.h`, `KTDX.h`, `OnlyGlobal/Always_*.h`.

## Variation axes

| Axis | Macros | Source |
|---|---|---|
| Country | `SERV_COUNTRY_{US,EU,JP,TW,HK,TWHK,CN,TH,ID,BR,PH,IN}`, `CLIENT_COUNTRY_*`, `SERV_INT_INTERNAL` | Per-configuration vcxproj |
| Environment | `_SERVICE_`, `_IN_HOUSE_`, `_IN_HOUSE_SERVICE_READY_QA_`, `_OPEN_TEST_`, `_OPEN_TEST_2_`, `_DEBUG`, `X2TOOL`, `EFFECT_TOOL` | vcxproj + `ServerDefine.h:4389` |
| Global | `SERV_GLOBAL_COMMON` (+ `SERV_GLOBAL_AUTH/BILLING/FRIEND/CASH_PACKAGE`) | `ServerDefine.h:4608` → includes `OnlyGlobal/ServerDefine/ServerDefine_<country>.h` |
| Features | `SERV_<FEATURE>` — events, systems, dated fixes | `ServerDefine.h`, `ServerDefine_<country>.h` |
| Build | `BUILD_BY_HUDSON`, `_CONVERT_VS_2010` | vcxproj |

The client has ~40 configurations in `X2Lib.vcxproj` (`US_INTERNAL`, `EU_SERVICE`, `JP_TEST`...).

## Status (2026-10-29)

Server resolved: [Flags and verification](../tools/flags.md) (736 true, 423 false, differential verification across the entire tree). Client: pending.

## Strategy: freeze with a tool, not by hand

1. Selected variant: US / `_SERVICE_` (decision T5 in [Decisions](../project/decisions.md)).
2. Extract the effective set of defined macros: preprocess `ServerDefine.h`/`Always.h` with `-dM -E`, using the target vcxproj defines.
3. Resolve using the custom resolver (`tools/import/x2import/condres.py`) instead of `coan`/`unifdef`; it removes dead branches and the directives themselves.
4. Delete `ServerDefine_<other countries>.h`, `OnlyGlobal/*/<other countries>`; reduce `ServerDefine.h` to a small file (or remove it).
5. Compile and compare behavior (smoke test) before and after.

**Debug exception:** whitelisted development macros and direct `_SERVICE_`/`_IN_HOUSE_` checks are not resolved;
they become `#if X2_DEV_TOOLS`. See [Debug and development](debug-and-development.md).

Precautions:
- Macros used in expressions in `#if` (`defined(A) && !defined(B)`) — `coan` handles them well; `unifdef` does too.
- Macros **defined and then `#undef`ed** (e.g. `SERV_INT_INTERNAL` disables `SERV_TIME_DROP_MONSTER_EVENT`) — use the final state.
- Some flags may be in resource `.lua` files or SQL — they are not affected by `coan`.
