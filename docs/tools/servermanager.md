[Home](../README.md) › [Tools](README.md) › ServerManager

# ServerManager (`src/tools/ServerManager`)

SDL3 + ImGui tool that starts, monitors and controls the servers, locally and over SSH (decisions T29, T30, T32). The servers have no
GUI or admin port: ServerManager speaks their [control protocol](../porting/console.md#protocol) (JSON lines) through a child
process started with `--machine`, through a local control socket, or through `ssh host x2ctl attach <name>`. Started on 2026-11-04;
progress and the remaining steps are in the [roadmap](../project/roadmap.md#phase-2--server).

## Layout

| Part | Target | Contents |
|---|---|---|
| `core/` | `x2_servermanager_core` | No GUI, tested headless: protocol messages (`protocol.h`), a session per server (`session.h`), logs (`log_store.h`), transports (`transport.h`), profile (`profile.h`), and orchestrator (`orchestrator.h`) |
| `core/tests/probe.cpp` | `x2_servermanager_probe` | Manual test of the orchestrator against real servers, without GUI |
| (UI, later step) | `x2_servermanager` | The ImGui window on [`x2::app_imgui`](../porting/app.md) |

The JSON parser (`x2::platform::ParseJson`, full documents) lives in [`x2::platform`](../porting/platform.md); the servers keep
reading commands with the flat `ParseJsonObject`.

## Core (2026-11-04, `tools/servermanager-protocol`)

- **`ParseServerMessage`** turns one line into `hello`, `log`, `output`, `status` or `result`. Anything else (not JSON, an unknown
  `type`, an `ssh` error message) is a `RawLine`, shown as output rather than dropped.
- **`ServerSession`** is one server, independent of its transport: the transport reports `OnConnecting`/`OnConnected(sender)`/
  `OnLine`/`OnDisconnected`, and the session keeps the link state, the server state (`hello.state`, then `status`), the name and PID
  from `hello`, and the requests in flight. `RunLua`/`Stop` send `{"id":n,...}` and match the `result` by `id`; on disconnection,
  pending requests fail with `disconnected`. A `hello` with another protocol version is noted as a warning but still read.
  Not thread-safe: transports read on their own threads and hand lines to the UI thread.
- **`LogStore`** keeps every entry of a server in arrival order with a sequence number: server logs, loose output, commands typed in
  ServerManager with their results (results with no output are not logged), and events noted by ServerManager (connection, state).
  It is bounded (100,000 entries by default; the oldest are dropped). **`LogView`** is a filtered list of sequence numbers (levels,
  kinds, ASCII case-insensitive search over message, function, file, fields and text, and a function filter), updated incrementally.
- **Log history** (2026-11-05, `server/control-history`): when a `hello` carries `seq` > 0 (an attach to a server that already
  logged), the session sends `{"id":n,"history":true,"upto":seq}` without echoing it as a command
  ([protocol](../porting/console.md#history)). Until its result arrives, log and output lines are held; then the backlog and the
  held live lines are added together in `seq` order, a line that came both ways only once, after an event
  `history: N earlier lines`. Events noted meanwhile (connection, state) stay before them. A disconnection adds what was held.
  `SetReplayHistory(false)` turns it off. A child started by ServerManager reports `seq` 0 and asks for nothing. The backlog is
  bounded on the server, so a server that has run for a long time only replays its latest lines.

## Local processes and orchestration (2026-11-05, `tools/servermanager-local`)

### Transport

Every server is reached through a **child process** speaking protocol lines on its stdin/stdout
([`x2::platform::Process`](../porting/platform.md)):

| Case | Child |
|---|---|
| A server ServerManager starts | `<program> <args> --machine [--control=<name>]` in the server's folder |
| A server already running on this machine (service, another terminal) | `x2ctl attach <name>` |
| A remote server (step `tools/servermanager-ssh`) | `ssh host x2ctl attach <name>` |

A socket client was not used for local servers: `ControlClient::ReadLine` blocks, and on Windows closing a socket from another thread
does not wake it ([Server console](../porting/console.md#control-socket)). Ending a child always ends its readers, and the three cases
behave the same. `ProcessTransport` reads stdout and stderr on two threads into an `Inbox` (each event tagged with the server and a
connection generation, so a late event from an earlier connection is ignored) and reports one `Closed` with the exit code. Lines on
stderr (a loader error, a crash message, an `x2ctl` error) become output entries.

- Closing the stdin of a `--machine` child shuts the server down (end of input); closing the stdin of `x2ctl attach` only disconnects.
- When ServerManager exits, the servers it started get the end of stdin and up to 5 seconds before being killed; attached servers
  keep running.

### Profile

A JSON file (`LoadProfile`) or the built-in default for the package under `dist/` (`DefaultProfile`):

```json
{"x2ctl": "./x2ctl",
 "servers": [
   {"name": "EPublisherServer", "directory": "EPublisherServer", "program": "./EPublisherServer", "args": ["DEV"]},
   {"name": "GameServer", "directory": "GameServer", "program": "./GameServer", "args": ["DEV", "0"],
    "after": ["LoginServer", "CenterServer", "GlobalServer"], "control": "GameServer"}]}
```

- Relative `directory` and `x2ctl` paths are relative to the profile's folder; `program` is relative to `directory` (on Windows `.exe`
  may be left out). `args` come before the console options ServerManager adds.
- `after`: servers that must be **ready** first. `control`: the control socket name, by default the program's file name (the
  server's own default); a second GameServer needs another one, and ServerManager passes it as `--control=`.
- Checked when loaded: unique names, a program for each, known dependencies, no cycles.
- Default order (2026-11-05), from the original and the servers' connections: EPublisherServer → LoginServer, CenterServer,
  GlobalServer → GameServer → ChannelServer. The original `KX2ServerLauncher` waited for the `CenterServer_Ready` window before
  starting GameServer; GameServer retries the servers it connects to, but players must not reach a GameServer before its
  dependencies, so ChannelServer (the client's entry) comes last.

### Orchestrator

`Orchestrator` (UI thread: `Update()` after `Events()` wakes it) keeps one `Server` per profile entry with its `ServerSession` and a
phase: `offline`, `waiting` (for dependencies), `starting` (connected, not ready), `ready`, `stopping`, `failed`.

- **Start** (`StartAll`, `Start(i)` with its dependencies): a server connects once every dependency is `ready`. If its control socket
  already exists here it is **attached** instead of started a second time. A dependency that failed or will not start cancels the
  servers waiting for it.
- **Stop** (`StopAll`, `Stop(i)`): `{"stop":true}`, in reverse dependency order: a server is stopped once no server being stopped still
  depends on it. A server ServerManager started that is still running after `stopGrace` (60 s) is terminated (POSIX SIGTERM; Windows
  closes its stdin), then killed after `killGrace` (10 s). A server stopped from elsewhere (`x2ctl stop`, its own prompt) is followed.
- `Attach`/`Detach` (attached servers stay running), `Kill` (servers ServerManager started).
- A child that exits, or a connection that closes, without a stop request is **failed** with the reason (`exited with code 5`); no
  automatic restart.

Tested on 2026-11-05 in the container with the six real servers (`x2_servermanager_probe dist`): attached to servers already running
in terminals and stopped them in reverse order (12.6 s); then started all six as children, all ready in 19.4 s, and stopped them in
17 s, every exit code 0.

## Tests

`x2_servermanager_tests` (`ctest -R servermanager`): recorded protocol lines, log store/filter/view, session with a fake transport,
and a session following a real `KServerConsole` in service mode over its control socket (hello, `ready`, a `START_LOG` with a field,
Lua results, a second client attaching late that gets the earlier lines in order and once each while a live one arrives, stop
until disconnection; the stop request is process-wide, so a second console cannot run in the same test process). Profile parsing, validation and order. Orchestrator with real children: three
`x2_terminal_demo` instances started in dependency order and stopped in reverse, a crash (`--exit-after`) that fails the server and
cancels its dependent, a missing program, a child that ignores stop (`x2_process_helper sleep`: terminated, then killed), and a
service already running that is attached through `x2ctl`, detached, attached again and stopped. Socket directory:
`$XDG_RUNTIME_DIR/x2`. A failure prints each server's phase and last log entries.

`x2_servermanager_probe <dist | profile.json> [--attach-only]`: starts (or attaches to) every server, waits until all are ready,
stops them, and prints each phase change with its time.
