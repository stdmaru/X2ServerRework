[Home](../README.md) › [Original code](README.md) › Original code encoding and conventions

# Original code encoding and conventions

## Encoding

Sample of ~400 `.cpp` files (server + client) inspected with `file`:
- Most are reported as "ISO-8859" / "Non-ISO extended-ASCII" → effectively **CP949 (EUC-KR)**: Korean comments and string literals.
- A minority are already UTF-8 (some with BOM, e.g. `luajitLib/ReadMe.txt`), and some are plain ASCII.
- Mixed line endings: CRLF, LF, and CRLF+LF in the same file; one case with `NEL`.

Suggested conversion (Phase 0):
1. Detect per file (`uchardet`/`charset-normalizer`): keep ASCII/UTF-8; convert the rest with `iconv -f CP949 -t UTF-8`.
2. Review diffs of **string literals** (`L"..."`, `"..."`) — MSVC compiled them with code page 949, so changing to UTF-8 changes the binary.
   Player-visible strings usually come from tables (`StringID_def*.h`, `.lua`, `.kom`), not code, but verify.
3. Normalize to LF + `.gitattributes`; add `.editorconfig`.
4. Compile with `/utf-8` in MSVC.

Read original files in a terminal: `iconv -f cp949 -t utf-8 -c <file>`.

## Observed conventions

- `K` prefix (KNC = KOG company / "KNC"): `KActor`, `KGSUser`, `KTDX*` (engine), `CX2*` (game, client).
- Change blocks marked with comments `//{{ author : date` ... `//}}` and feature headers in `ServerDefine.h`
  (`name/id/work start date/patch date/work description`). These are SVN history embedded in the code → candidates for removal along with the flags.
- Large amounts of commented-out code (e.g. several commented `uncompress` calls in `KGCMassFileManager.cpp`).
- Precompiled headers `stdafx.h` in every project.
- Old project formats coexist: `.vcproj` (VS2003/2005), `*_2003.vcxproj`, `.dsp/.dsw` (VC6), `.vspscc/.vssscc` (SourceSafe) → clutter.
