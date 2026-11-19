# V5 merge scripts (temporary, PR 112)

Throwaway helpers used to bring the new job paths from V5 (`/media/games/Elsword-V5.0318.6_3`, V5 = `379dc65d`,
V4 = `ae28cbe8`) into `src/client/X2Lib`. They hardcode local paths and write to a `m3/` folder next to `py/`
(adjust `M3`/`REPO`). **Delete this folder before PR 112 is marked ready.**

- `merge3.py <file>`: 3-way merge, base = `_reference` resolved with the importer flags (`--v4-base` for V4), ours = src,
  theirs = V5; writes `m3/<file>` (diff3).
- `gate.py <file> [--write]`: keeps only merge changes naming the new paths (IDs from the `#ifdef X2_*_PATH` blocks of
  X2SkillTree.h/X2Unit.h/X2ComboTree.h plus `PAT`), guards them, leaves `//@@CONFLICT n` markers; `.conflicts`/`.dropped`.
- `autoconf.py <file>`: resolves the markers by appending V5's added chunks, guarded.
- `resolve.py <file> <n> <textfile|->`: replaces one marker by hand.
- `v5apply.py`, `dedup_ifdef.py`: hunk applier for small files; removes nested same-switch `#ifdef`s.
