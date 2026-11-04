# AGENTS.md

Instructions for agents and people working in this repository.

## The project

Port the Elsword server and client (`_reference/X2Server-github`), which currently targets only Windows/DX9/MSVC, to Windows
and Linux with CMake and x64, cleaning up the code along the way. Start with the libraries, then move on to the server and,
finally, the client.

## Before starting any task

1. Read the [wiki home page](docs/README.md) and the [Project](docs/project/README.md) pages:
   [Roadmap](docs/project/roadmap.md) (work in progress) and [Decisions](docs/project/decisions.md) (decisions already made).
2. Before changing a module or topic, open its wiki page: [Port](docs/porting/README.md) for modules under `src/`, and
   [Original code](docs/original/README.md) for the behavior of `_reference`.
3. Do not reopen a decision recorded in `decisions.md`. If something new conflicts with a decision, point it out to the user.
4. Consult the wiki whenever context is needed. Before investigating something from scratch, check whether it is already documented.
5. When changing code, make sure the wiki stays up to date.

## Fixed rules

- **`_reference/` is read-only.** Never edit, format, or commit files there. `_reference/X2Server-github` points to a verified
  clone of `github.com/stdmaru/X2Server`.
- **`_reference/` will be deleted at the end of the project.** Nothing (builds, execution, tests, or tools) may depend on it,
  except the importer when bringing in files. Everything the servers need at runtime is versioned under `runtime/`, except
  binaries and client assets (decision T22).
- **Original code enters `src/` only through the importer** (`tools/import`; see [Importer](docs/tools/importer.md)).
  Once an imported file has been edited by hand, do not reimport that module. Fix it directly in `src/`.
- **Client assets must not change** (`_client/`, `.kom`, `.dds`, `.x`, `.fx`, ...). Adapt the code to them.
- **Keep the protocol compatible with the original client** (`x2.exe`): do not change packet formats, sizes, or cryptography
  without a recorded decision. Types sent over the network must have fixed sizes (`int32_t`, `char16_t`, ...). See
  [Protocol](docs/original/protocol.md).
- **Only one variant: US / `_SERVICE_`.** Debug/dev/admin tools are gated by `X2_DEV_TOOLS` (see
  [Debug and development](docs/original/debug-and-development.md)). Discard other countries, anti-cheat, publisher integrations,
  and legacy code.
- **64-bit only: x86_64 on Windows and Linux, and arm64 on Linux.** Linux builds use Clang (GCC is an additional check);
  Windows uses clang-cl. All build from the same CMake project (decision T26). `_WIN32` paths are compiled and tested on a
  Windows machine over SSH (`tools/win/winbuild.sh`) and in the CI **clang-cl** job. No x86 assembly/intrinsics, do not
  assume `char` is signed or unsigned (the build forces `-fsigned-char`), and do not use `volatile` for thread synchronization.
- `windows.h` and system APIs belong only in `src/platform` (`x2::platform`). Ported code includes
  `x2/platform/win32.h` instead of `<windows.h>`, `<atltime.h>`, or `<tchar.h>`. See [Platform](docs/porting/platform.md).
- Modules marked `frozen = true` in `tools/import/modules/` have already been ported by hand; the importer refuses to write
  to them.
- Builds must be warning-free (GCC, Clang, and clang-cl). Fix new warnings; do not suppress them.

## Layout

```
src/            new code (platform/, kncsdk/, then server/ and client/)
cmake/          project CMake modules
tools/import/   _reference -> src/ importer (+ reports in reports/, manifests/)
tools/docs/     wiki utilities
tools/kom/      client .kom/.kim tools (extract, decompile, package)
tools/runtime/  generate ClientScript from the client
runtime/        server runtime files (ServerResource, configs, and .dsn templates)
docs/           project wiki
_reference/     original code (read-only, outside git)
_client/        original client and assets (outside git)
_database/      SQL Server game database backups (outside git)
docker/         Ubuntu build image (build/, build.sh) and local services (SQL Server)
```

## Commands

Builds and tests run in the project's Ubuntu image (decision T24): prefix build/test commands with `docker/build.sh`
(with no arguments, it opens a shell in the image). See [Build environment](docs/tools/build-environment.md).

```bash
docker/build.sh cmake --preset linux-debug                            # configure (dev tools ON)
docker/build.sh cmake --build --preset linux-debug                    # build
docker/build.sh ctest --preset linux-debug                            # tests
docker/build.sh cmake --preset linux-gcc && docker/build.sh cmake --build --preset linux-gcc   # GCC check
X2_WIN_HOST=<user@host> tools/win/winbuild.sh                        # Windows (clang-cl) test machine over SSH
gh workflow run ci.yml --ref <branch>                                # run Windows CI manually; minutes are limited
python3 tools/import/import.py <module> --dry-run                     # importer
python3 tools/import/tests/test_condres.py                            # importer tests
python3 tools/import/tests/test_flagsim.py
python3 tools/import/gen_flags.py server                              # generate server flags
python3 tools/import/check_equivalence.py server                      # verify resolver does not change code
python3 tools/docs/check_links.py                                     # wiki links
docker compose -f docker/docker-compose.yml up -d --wait              # development SQL Server
docker/mssql/restore.sh                                               # restore _database/*.bak to it
docker/build.sh cmake --build --preset linux-debug --target x2_run_channel  # create dist/ChannelServer
docker/build.sh cmake --build --preset linux-debug --target x2_run_epublisher  # likewise, EPublisherServer (DEB login/cash, T23)
```

The original code is encoded in CP949. To read it, use `iconv -f cp949 -t utf-8 -c <file>`. Some files are already UTF-8.

## Code

- Write new code and code comments in English. Project documentation is maintained in English. Translate Korean comments from the
  original when editing or copying that code. Add comments only when they help.
- Follow the style of the surrounding code. When cleaning up, prefer the C++20 standard library to Boost or Win32.
- Any behavior change from the original must be intentional and recorded on the module page under `docs/porting/`.

## Documentation

The wiki under `docs/` is the project's memory. It is organized as a **navigable wiki with indexes**:

- **Home page:** `docs/README.md`. Introduces the project, points readers to where to start, and lists the areas.
- **Areas are folders**, each with a `README.md` index. Current areas:
  `project/` (direction: mission, roadmap, decisions), `original/` (analysis of `_reference`),
  `porting/` (notes for each `src/` module), and `tools/` (project tools).
- **Topics that grow become subfolders** with their own index (`README.md`), continuing down the hierarchy.
  Example: `porting/server/README.md` → `porting/server/gameserver.md`.
- **Every new page must be added to its folder's index.** An unindexed page does not exist.
- **Every page starts with navigation from the root:** `[Home](../README.md) › [Area](README.md) › Title`.
- **Indexes distinguish essential content from reference/noise**, so readers know what they need to read and what is just a reference.
- **One piece of information, one place.** Do not duplicate information; link to the page that owns it.
  Decisions belong only in `project/decisions.md` (`T#` decided, `D#` pending), and progress belongs only in
  `project/roadmap.md`.
- **Keep the wiki up to date as part of the same task:** when completing a step, update the roadmap; when learning something
  about the original, record it on the relevant area page; when porting a module, update its `porting/` page.
- Use relative links. Use absolute dates (`2026-10-29`), never “yesterday” or “last week”.
  Counts and estimates must include the date they were made.
- After moving or creating pages, run `python3 tools/docs/check_links.py`.

## Git

### Task workflow

1. **Plan:** Understand the objective, scope, constraints, and relevant project documentation. Identify the implementation approach before making changes.
2. **Branch and PR:** Before implementation, create a short-lived branch for the task, push it to `origin`, and open a draft PR targeting `main`. The PR is the task's living record.
3. **Work:** Implement the task in small, meaningful commits. Push each commit and keep the PR description, checklist, and progress updated.
4. **Handoff:** The PR must always reflect the current state of the work, including completed items, remaining work, relevant findings, blockers, and next steps. It must be sufficient for another agent or session to resume without repeating the investigation.
5. **Review:** When the task is complete, all relevant documentation is updated, and validation passes, update the PR and mark it ready for review. Do not merge it.

If interrupted, commit and push current progress, then update the PR with completed work, remaining tasks, blockers, and next steps. Never leave uncommitted work as the only record of progress.

- Commit message format: `type: summary` (`feat`, `fix`, `docs`, `refactor`, `test`, `build`, `ci`, `chore`), with a body explaining why.

### Branches

- **`main` is always green**: builds without warnings (GCC, Clang, and clang-cl), `ctest` passes (Linux and Windows in CI), and
  `tools/docs/check_links.py` reports no issues. Never work directly on it.
- **Use one short-lived branch per roadmap step**, named `area/topic`. Areas: `platform`, `kncsdk`, `server`, `client`, `tools`,
  `docs`, `build`. Examples: `tools/server-flags`, `server/common-import`, `build/windows-clang-cl`.
- If `main` has moved, rebase the branch onto `main` and push the updated work branch before continuing.
- Before requesting integration, run a clean build, tests, the link checker, and Windows validation
  (`tools/win/winbuild.sh` or a CI run). Integrate through the PR after its checks pass and it is approved; preserve linear history
  by using GitHub's rebase-and-merge option. Do not merge the PR locally or push its changes directly to `main`.
  Actions has a **2,000-minute monthly budget** (Windows counts twice); CI runs automatically only on `main`. See
  [Continuous integration](docs/tools/ci.md#minutes-orçamento). Delete the work branch after the PR is merged.
- If work is too large for one step, split it into sequential branches rather than keeping one long-lived branch.
