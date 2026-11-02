# X2ServerRework

Port of the **Elsword** servers (and, later, the client) to **64-bit Windows and Linux**.

The original code only runs on Windows: MFC, DirectX 9, and 32-bit MSVC. Here, it is being moved to C++20 and CMake,
built with Clang on both platforms, and cleaned up along the way. The **protocol remains compatible with the original client**:
the ported servers work with the original `elsword.exe`, without changing the client or its assets.

## Status (2026-10-31)

| Server | Role | Linux | Windows |
|---|---|---|---|
| EPublisherServer | login and cash | runs | builds |
| ChannelServer | client's first connection: login, servers, and channels | runs | builds |
| LoginServer | accounts, guilds, rankings | runs | builds |
| CenterServer | field and dungeon rooms | runs | builds |
| GameServer | characters, towns, fields, dungeons, items, quests, PvP | runs | builds |
| GlobalServer | global missions, automatic matchmaking, PvP, user counts | runs | builds |

Linux builds are supported on x86_64 and arm64. The full build and test suite have passed on arm64, and LoginServer
authentication has been verified there. The GitHub Actions workflow includes a Linux ARM64 build/test job.

All original servers have been ported. With the original client, login, character selection, the town, fields, and a dungeon
are already working with all six servers running together.
Everything builds without warnings and the tests pass on Windows, but the servers have only been run on Linux.

## Build

- [Linux](docs/building-linux.md): using the project's Docker container (recommended) or natively, with packages for each distribution.
- [Windows](docs/building-windows.md): Visual Studio 2026 with clang-cl.

## Run

[Run the servers](docs/running.md): the database (SQL Server in Docker), local settings (`docker/.env`), generated run
directories, server startup order, and the client.

The original client and database backups are not part of this repository.

## What's next

A new server terminal, structured logging, and **ServerManager**, a tool to orchestrate and monitor the servers locally and over SSH.
Next comes the client port to SDL3 + SDL_GPU. See the [Roadmap](docs/project/roadmap.md) for all planned work.

## Documentation

The [project wiki](docs/README.md) contains the details: decisions, progress, notes on each ported module, and analysis of the
original code. Before working on the code, start with [AGENTS.md](AGENTS.md), which contains the repository rules (style,
branches, and what must not be changed).
