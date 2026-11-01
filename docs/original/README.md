[Home](../README.md) › Original code

# Original code (`_reference/X2Server`)

Analysis of the reference code, performed on 2026-10-29 at commit `b8fb1380`.
This is reference material; decisions resulting from it are recorded in [Project › Decisions](../project/decisions.md).

## Overview

| Page | Contents |
|---|---|
| [Inventory](inventory.md) | Modules, sizes, dependency graph, data |
| [Libraries](libraries.md) | Each library/third party: version and action (replace/port/remove) |

## Platform dependencies

| Page | Contents |
|---|---|
| [Server](server.md) | IOCP, ODBC, MFC, threads, time |
| [Client](client.md) | D3D9/D3DX, input, audio, IME, video |
| [Protocol](protocol.md) | Serialization and network compatibility (`long`, `wchar_t`) |
| [Proprietary binaries](proprietary-binaries.md) | Source-less `.lib`/`.dll`: anti-cheat, billing, publishers |

## Cleanup

| Page | Contents |
|---|---|
| [Feature flags](feature-flags.md) | `SERV_*`, countries, environments: the largest volume of dead code |
| [Debug and development](debug-and-development.md) | Debug/admin tools to retain (`X2_DEV_TOOLS`) |
| [Encoding](encoding.md) | CP949, line endings, original code conventions |
| [Original bugs](bugs.md) | Original incorrect behavior retained in the port: Phase 5 backlog |

## Out of scope

| Page | Contents |
|---|---|
| [Out of scope](out-of-scope.md) | Tools and projects not being ported for now |
