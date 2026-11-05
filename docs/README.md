# X2ServerRework — Wiki

Port of the X2 server and client (`_reference/X2Server`), which currently targets only Windows/DirectX 9/MSVC,
to a cross-platform codebase (Windows + Linux), with cleanup and reorganization along the way.
Client assets (`.kom`, `.dds`, `.x`, `.fx`, ...) are **not** modified: this is a code-only port.

## Start here

1. [Mission and scope](project/mission-and-scope.md): what is in scope and what is not.
2. [Roadmap](project/roadmap.md): project phases and current progress.
3. [Decisions](project/decisions.md): decisions made and pending.

## User guides

For those who just want to build and run the project (also linked from the root [README](../README.md)):

- [Build on Linux](building-linux.md): using Docker or building natively, with packages for each distribution.
- [Build on Windows](building-windows.md): Visual Studio 2026 with clang-cl.
- [Run the servers](running.md): database, local settings, the server package, servers, and client.

## Areas

| Area | Contents |
|---|---|
| [Project](project/README.md) | Mission, roadmap, and decisions. **Essential.** |
| [Original code](original/README.md) | Analysis of `_reference`: modules, libraries, Windows dependencies, protocol, flags, and debugging |
| [Port](porting/README.md) | Work notes for each ported module: status, changes, and open items |
| [Tools](tools/README.md) | Project tools (importer) |

## Wiki conventions

- Each area is a folder with a `README.md` index. Larger topics get their own subfolder and index.
- Every page starts with a navigation trail (`Home › Area › Page`).
- Indexes distinguish **essential** content from **reference/noise**.
- Counts (lines, files, etc.) are rough estimates, with the date noted on the page.
- Complete rules for editing the wiki are in [AGENTS.md](../AGENTS.md#documentation).
