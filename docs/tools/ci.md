[Home](../README.md) › [Tools](README.md) › Continuous integration

# Continuous integration (GitHub Actions)

`.github/workflows/ci.yml`, created on 2026-10-30 (Arch Linux container; moved to Ubuntu image the same day, T24, and MinGW + Wine
was replaced by clang-cl on Windows, T26).

## Minutes (budget)

Repository is private: **2,000 Actions minutes/month**, with Windows minutes charged at **2×**. Before the ARM64 job was added,
a full run cost ~80–110 billed minutes as of 2026-10-30 (Linux jobs 16–22 min with cold ccache, Windows 14 min × 2). The current
total is higher due to the additional Linux ARM64 job. Therefore:

- **Automatic only on push to `main`** (when integrating), not when push changes only `docs/`, `*.md`, or `.claude/`.
- **Work branches:** run Linux matrix locally for free (`docker/build.sh`, [Build environment](build-environment.md)). Trigger CI manually
  only as needed, by default **Windows only**, when Windows test machine is unavailable:
  `gh workflow run ci.yml --ref <branch>` (or `-f jobs=linux` / `-f jobs=all`). Then `gh run list --branch <branch>`.
- **Before integration** (cadenced validation, user, 2026-11-12): every PR passes the local Linux Clang build (no warnings),
  `ctest` and the link checker. GCC and Windows (`tools/win/winbuild.sh`, [Build environment](build-environment.md#windows-clang-cl-over-ssh))
  run locally only when the PR is at risk there: `_WIN32` paths, `src/platform`, system or window code (DXUT, SDL), CMake,
  new names that may clash with Windows macros (`far`, `near`, `min`, `max`...), and at milestones (the end of a group of
  roadmap steps). Otherwise the CI on `main` checks them after the merge: merging several PRs in a row runs it once for
  the batch (`cancel-in-progress` cancels the earlier runs), and a failure on `main` is fixed by the next PR before other
  work. **Do not wait for CI on PRs** (user, 2026-11-05); ARM64 is only covered by CI.
- **Linux matrix has 3 entries:** Clang Debug on x86_64 and ARM64, plus GCC Release on x86_64 (dev tools OFF; verification only).
  Clang Release, the distributed build, stays in the local matrix.
- **`timeout-minutes`** on all jobs; a new push to same branch cancels previous run.
- Branches restore ccache data only from `main`, so they benefit from a warm cache after `main` has run.

Linux jobs use the [build environment](build-environment.md) image (Ubuntu, T24), same as `docker/build.sh`; Windows job uses a
Windows runner (T26). Linux CI includes native x86_64 and ARM64 runners.

| Job | What it does |
|---|---|
| Linux x86_64: Clang Debug, GCC Release (check); Linux ARM64: Clang Debug | Builds `linux-debug` on x86_64 and ARM64 and `linux-gcc` on x86_64 (verification only), then runs `ctest` for each job |
| clang-cl Debug | `windows-latest` runner (outside Ubuntu image): Visual Studio x64 environment (`ilammy/msvc-dev-cmd`) and runner LLVM clang-cl, `windows-debug` preset (`-k 0`: all errors in one pass), `ctest`. Distributed Windows build (T26) and only place Windows is compiled; LuaJIT via `msvcbuild.bat` (T25). Windows minutes count 2× for private repos |
| Importer and wiki | `tools/import/tests/test_condres.py`, `test_flagsim.py`, and `tools/docs/check_links.py` |

- **No warnings**: build log is saved and any `warning:` (`cl`/`link`, `warning C####:`/`LNK####:`) fails job (AGENTS.md rule).
- **Dependencies**: image packages; LuaJIT via pinned-commit CMake recipe (T21); Asio, zlib, and doctest through pinned `FetchContent` (T13).
  `ccache` persisted between runs (`actions/cache`).
- **Outside CI**: `KODBC` `db` suite (requires SQL Server: `X2_TEST_ODBC`, see [Development database](development-database.md)),
  `ClientScript` generation (requires client, see [Server package](runtime.md)). Repository has no
  `_reference/`, `_client/`, or `_database/`, and CI depends on none of them.

View results: `gh run list`, `gh run view --log-failed`.
