[Home](../README.md) › [Tools](README.md) › ServerManager

# ServerManager (`src/tools/ServerManager`)

SDL3 + ImGui tool that starts, monitors and controls the servers, locally and over SSH (decisions T29, T30, T32). The servers have no
GUI or admin port: ServerManager speaks their [control protocol](../porting/console.md#protocol) (JSON lines) through a child
process started with `--machine`, through a local control socket, or through `ssh host x2ctl attach <name>`. Started on 2026-11-04;
progress and the remaining steps are in the [roadmap](../project/roadmap.md#phase-2--server).

## Layout

| Part | Target | Contents |
|---|---|---|
| `core/` | `x2_servermanager_core` | No GUI, tested headless: protocol messages (`protocol.h`), a session per server (`session.h`), and logs (`log_store.h`) |
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
- **Log history:** the protocol does not replay past logs, so a session attached to a server that has been running for a while
  starts with what comes after `hello`. A child started by ServerManager is complete from its first line. Replaying a backlog is the
  `server/control-history` step.

## Tests

`x2_servermanager_tests` (`ctest -R servermanager`): recorded protocol lines, log store/filter/view, session with a fake transport,
and a session following a real `KServerConsole` in service mode over its control socket (hello, `ready`, a `START_LOG` with a field,
Lua results, stop until disconnection). Socket directory: `$XDG_RUNTIME_DIR/x2`.
