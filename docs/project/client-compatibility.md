[Home](../README.md) › [Project](README.md) › Client compatibility

# Client compatibility across source versions

Preliminary analysis of the 2014 server source in `_reference/X2Server-github` and the 2015 source in
`/media/games/Elsword-V5.0318.6_3/JP_TW_EU_CN_US/Trunk`, performed on 2026-11-03. This page records findings and a
proposed direction; it does not add 2015 code to `src/` or decide the final build interface.

## Goal

Keep one maintained codebase that can be compiled for a selected client/protocol revision, for example
`PROTOCOL_VER=20140328` or `PROTOCOL_VER=20150321`. Each build selects one revision. The goal is not for one running
server binary to negotiate or serve both protocols simultaneously.

The current 2014 port remains the working baseline. Later work should bring in selected 2015 changes while retaining
the ability to build and validate the 2014 configuration.

### Feature configuration (user direction, 2026-11-05)

The user wants game content and features to be switchable at compile time, in the spirit of rAthena's
`PACKETVER`: a client revision (for example `CLIENT_VER 4` for 2014, `CLIENT_VER 5` for 2015) plus individual switches
such as `CLIENT_ENABLE_8TH_CHARACTER`, so that a character or a feature can be added or removed, and content added
after the 2015 source can be gated the same way. The switches live in **one configuration file**, split into what
affects the server, the client, or both; new project-specific defines are allowed. This is Phase 6 work and is not
designed yet.

It does not reopen T9/T10: the original ~2,600 KOG flags stay frozen at import (2014 baseline, US / `_SERVICE_`). The
new switches are a small, curated set added when the 2015 changes are ported, each one guarding code that differs
between revisions or that the user wants optional. Shared code (`Common`, T33) is compiled by both programs, so a switch
that changes a packet must be set the same way for the server and the client of one build.

## Preliminary findings

### The server code is substantially related, but the changes span modules

The comparison below counts source/data files (`.h`, `.hpp`, `.inl`, `.cpp`, `.c`, `.lua`, `.sql`) under the six server
modules. “Same path” means a file exists at the same relative path in both snapshots; “identical” means its bytes match.
The 2015 checkout contains additional regional variants, build products, and other non-source files, so raw directory
file counts are not a measure of porting effort.

| Module | 2014 files | 2015 files | Shared paths | Byte-identical shared files | Changed shared files |
|---|---:|---:|---:|---:|---:|
| `Common` | 729 | 718 | 715 | 629 | 86 |
| `ChannelServer` | 82 | 82 | 81 | 75 | 6 |
| `GameServer` | 387 | 397 | 387 | 324 | 63 |
| `CenterServer` | 160 | 163 | 160 | 126 | 34 |
| `GlobalServer` | 115 | 118 | 115 | 104 | 11 |
| `LoginServer` | 123 | 124 | 123 | 107 | 16 |

This indicates a useful shared base, but the differences are not isolated to the new-character implementation. `Common`
and `GameServer` contain the largest counts of changed shared files, and the other server modules also change.

The 2015 tree has concrete new gameplay code, including `Common/X2Data/XSLProfessionManager.{h,cpp}` and
`GameServer/GSUserProfession.{h,cpp}` and `GSUserCharCustom.{h,cpp}`. These names are evidence of added progression and
character-customization systems; they do not by themselves establish which of those features are required for the
target client or for the intended character.

### The 2015 source already uses feature-gated character changes

The 2015 source contains `SERV_9TH_NEW_CHARACTER` conditionals in shared structures and logic, including
`Common/CommonPacket.h` and `Common/X2Data/XSLUnit.{h,cpp}`. This is direct evidence that character support affects
shared packet/data definitions and that the original codebase used compile-time feature selection for some changes.
The 2015 source also contains many service and region flags. Those flags must not be mistaken for client-version
differences: comparisons and imports need to hold region/service selection constant (US / `_SERVICE_`, per project
scope) before attributing a change to the client revision.

The current port freezes most original feature flags during import. Supporting multiple client revisions will therefore
need an explicit version-selection layer; it should not restore all original service, publisher, anti-cheat, and debug
variants.

### Protocol compatibility is a high-risk boundary

Packet IDs and definitions are shared in the original `Common` module, and serialized layouts must match the client
exactly. The 2015 differences in `Common` include `ClientPacket`, `CommonPacket`, and event ID files. Even when a packet
change looks small, adding a field, changing an enum value, or changing a structure selected by a flag can alter the
wire format. Each protocol revision must be checked against its own original client; the existing protocol notes are
in [Network protocol and serialization](../original/protocol.md).

### Runtime data and database compatibility are part of the version

The two trees have large, differently organized `ServerResource` sets, and the project currently generates
`ClientScript` from the active client's `.kom` assets (decision T22). A server build for a client revision therefore
needs a matching, documented set of scripts and other server-consumed data. Code-only compatibility would not be
enough to establish a working version.

The initial comparison did not establish the schema delta between the two database snapshots or which migrations are
required. Database schema, stored procedures, seed/configuration data, and persistence semantics need a separate,
targeted comparison before claiming end-to-end 2015 support.

## Suggested implementation shape

1. Keep the current 2014 build as the baseline and record its selected protocol revision explicitly.
2. Add a single build setting for the selected client revision (for example, CMake cache variable
   `X2_PROTOCOL_VER`), validate supported values at configure time, and expose one generated configuration header.
   The user's `PROTOCOL_VER` example describes the intended selector; the final public name and date encoding remain
   open until implementation.
3. Keep identical behavior shared. Put version-specific packet IDs/layouts and unavoidable behavior changes behind
   small, named compatibility boundaries. Avoid accumulating unrelated version checks throughout gameplay code.
4. Compare each candidate 2015 feature in both original snapshots, then port it into the current architecture. Do not
   copy whole 2015 modules over the port: the current code has platform, build, dependency, and cleanup changes that
   are not present in the old source tree.
5. Associate runtime scripts/data and any database changes with the selected revision. Do not infer that the 2015
   server scripts are compatible with the current client assets.
6. Build both supported configurations and validate each against its matching original client. The 2014 build remains
   a regression target whenever shared protocol or game logic changes.

Whether to use preprocessor branches, version-specific source files, or small adapters should be decided per kind of
difference. Preprocessor conditions are suitable for compile-time packet layouts and constants; larger behavioral
differences are easier to reason about when isolated in a version-specific implementation with a shared interface.

## Next investigation needed

- Normalize both originals to the same US / `_SERVICE_` feature set, then produce a source-only diff report that
  excludes binaries, generated files, third-party code, regional variants, and build outputs.
- Inventory packet ID, packet structure, serializer, and handshake changes between the two selected client revisions.
- Trace the new character's create/select/login/save/load/gameplay paths through `Common`, `LoginServer`, `GameServer`,
  `CenterServer`, scripts, and relevant database procedures.
- Identify the exact matching 2014 and 2015 client builds and their assets; confirm how the selected revision is
  represented and validated during the client connection flow.
- Decide how CMake selects protocol revision and how builds keep their version-specific runtime data separate.

Until these items are investigated, the evidence supports the feasibility of a staged, compile-time multi-version
codebase, but not a precise estimate of the full 2015 compatibility work.
