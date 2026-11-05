[Home](../README.md) › [Tools](README.md) › ServerManager

# ServerManager (`src/tools/ServerManager`)

SDL3 + ImGui tool that starts, monitors and controls the servers, locally and over SSH (decisions T29, T30, T32). The servers have no
GUI or admin port: ServerManager speaks their [control protocol](../porting/console.md#protocol) (JSON lines) through a child
process started with `--machine`, through a local control socket, or through `ssh host x2ctl attach <name>`, and shows them in
one [window](#window). Started on 2026-11-04;
progress and the remaining steps are in the [roadmap](../project/roadmap.md#phase-2--server).

## Layout

| Part | Target | Contents |
|---|---|---|
| `core/` | `x2_servermanager_core` | No GUI, tested headless: protocol messages (`protocol.h`), a session per server (`session.h`), logs (`log_store.h`), transports (`transport.h`), profile (`profile.h`), and orchestrator (`orchestrator.h`) |
| `core/tests/probe.cpp` | `x2_servermanager_probe` | Manual test of the orchestrator against real servers, without GUI |
| `ui/` | `x2_servermanager_ui`, `ServerManager` | The [window](#window) on [`x2::app_imgui`](../porting/app.md): `ManagerUi` (a library, drawn headless in tests) and the program |

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
| A server on another machine ([remote servers](#remote-servers-over-ssh)) | `ssh <options> <destination> '<x2ctl> attach <name>'` |

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

- `controlDir`: where this machine's servers have their control sockets when not the default (servers in containers, see
  [another folder](../porting/console.md#control-socket)); passed to `x2ctl` as `--dir=` and used by the discovery. The servers
  ServerManager starts itself keep their own default unless `X2_CONTROL_DIR` is set in its environment (they inherit it).
- Relative `directory`, `x2ctl` and `controlDir` paths are relative to the profile's folder; `program` is relative to `directory` (on Windows `.exe`
  may be left out). `args` come before the console options ServerManager adds.
- `after`: servers that must be **ready** first. `control`: the control socket name, by default the program's file name (the
  server's own default); a second GameServer needs another one, and ServerManager passes it as `--control=`.
- `hosts` and a server's `host`: see [remote servers](#remote-servers-over-ssh).
- Checked when loaded: unique names, a program for each local server, known dependencies and hosts, no cycles.
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
- **Discovery** (`Discover`, then `Listed(i)`): this machine's control sockets at once, each host's in the background
  (`x2ctl list` over ssh, collected by `Update`; `HostAt(h)` has the names or the error). `Stop(i)` on a server that is not
  connected but is listed attaches to it and stops it through the protocol, so its shutdown is logged; `StopAll` therefore stops
  every listed server, not only the connected ones.
- A child that exits, or a connection that closes, without a stop request is **failed** with the reason (`exited with code 5`); no
  automatic restart.

Tested on 2026-11-05 in the container with the six real servers (`x2_servermanager_probe dist`): attached to servers already running
in terminals and stopped them in reverse order (12.6 s); then started all six as children, all ready in 19.4 s, and stopped them in
17 s, every exit code 0.

## Remote servers over SSH (2026-11-05, `tools/servermanager-ssh`)

A server with a `host` runs on that machine as its service (systemd, a terminal there): ServerManager **attaches and never starts
it**, so it does not depend on the SSH connection staying up. Starting it means attaching once its dependencies are ready, like a
local server that is already running; if it is not running, `x2ctl attach` fails and the server is **failed** with
`could not attach on <host>` (the ssh or `x2ctl` message is in its log). Stopping is the protocol's stop over the attached session,
followed until the connection closes; a remote server that does not stop is not terminated or killed.

```json
{"hosts": [{"name": "prod", "destination": "x2@10.0.0.5", "port": 2222, "x2ctl": "/opt/x2/dist/x2ctl"},
           {"name": "lab", "destination": "lab", "ssh": ["ssh", "-p", "2222", "-o", "BatchMode=yes"]}],
 "servers": [{"name": "GameServer", "host": "prod", "control": "GameServer", "after": ["CenterServer"]}]}
```

- `controlDir` (per host): the control socket folder on that machine, for servers in containers there
  (`"controlDir": "/opt/x2/docker/server/control/x2"` with `docker/server/docker-compose.yml`); the remote command becomes
  `x2ctl --dir=<folder> attach <name>`. Not resolved on this machine. The ssh user must be able to open it (see
  [another folder](../porting/console.md#control-socket)).
- `port`: the ssh port (`-p`, after the ssh options and before the destination); without it, ssh's own (22, or the `Port` of
  `~/.ssh/config`).
- `destination`: what ssh connects to (`host`, `user@host`, or a `Host` of `~/.ssh/config`; not starting with `-`). `x2ctl`: its
  path on that machine, by default `x2ctl` in its `PATH`. A remote server needs `control` (or `program`, whose file name is the
  socket name); its `directory` is not resolved.
- `ssh`: the program and options before the destination. Without it, `DefaultSshCommand()`:
  `ssh -T -o BatchMode=yes -o ConnectTimeout=10`, plus on Linux `-o ControlMaster=auto -o ControlPath=<control dir>/ssh-%C
  -o ControlPersist=60`, so the attaches and queries to one host share a connection (Windows' OpenSSH has no ControlMaster).
  `BatchMode` makes a missing key fail instead of asking for a password, since there is no terminal: use keys or an agent.
- The remote command is one line for the remote shell, each word quoted once (`QuoteShellArgument`): a POSIX shell on the host.
- Authentication and encryption are SSH's (T30); the control socket on the host accepts only its own user, so `destination` must
  log in as the user the servers run as.

Tested on 2026-11-05 with the real ssh to `localhost` on the user's desktop (key authentication, the default command with
ControlMaster) and two `x2_terminal_demo` services: discovery listed both, the second attached after the first was ready, a third
server that was not running failed with the message above, and both were stopped in reverse order with exit code 0.

## Window (2026-11-05, `tools/servermanager-ui`)

```bash
build/linux-debug/src/tools/ServerManager/ui/ServerManager dist          # or a profile.json
```

Without an argument it opens `servermanager.json`, else `dist/`, in the current folder; another profile can be loaded from the
toolbar. Built in the container, it runs on the desktop like `x2_app_demo`. On Linux the build also writes
`ServerManager.desktop` next to the program (`Terminal=false`, opening `X2_DIST_DIR`), so it starts from a launcher without a
terminal; copy it to `~/.local/share/applications/`. On Windows it is a GUI-subsystem program (`WIN32_EXECUTABLE`, `WinMain` from
`SDL_main.h`): no console, and errors before the window go to a message box.

- **Toolbar:** profile path and Load, Start all, Stop all, Refresh (discovery, otherwise every 10 s), and each host's discovery
  state (green: listed; red: failed, with the error in its tooltip).
- **Servers** (left, resizable): name (`@host` for remote ones), phase (`running` when not connected but its socket is listed),
  and the error or warning count. For the selected server: Start (`Attach when ready` for a remote one), Stop, Attach, Detach,
  Kill, the server state, PID, how it is connected, the last exit code, the error, and its dependencies.
- **Log** (right): level and kind checkboxes (`Prompt` covers commands and results), search, function filter, Follow, Clear, and a
  count. One row per line (fields and text lines indented below their entry), drawn with `ImGuiListClipper`, so 100,000 entries
  stay cheap; rows are built incrementally and only rebuilt when the filter changes or the log is cleared. Hovering a log shows its
  function, stream, file and line; its context menu copies it or filters on its function. Scrolling up stops following; scrolling
  back to the bottom resumes.
- **Prompt:** Lua for the selected server, Enter or Run, Up/Down for its history (per server, in memory). Commands and results go
  to the log.
- **Quitting:** with no server started here running, the window closes (attached servers are detached and keep running).
  Otherwise it asks; "Stop them and quit" stops the servers it started, as Stop does, and quits once they are down ("Quit now"
  kills them).
- The event loop sleeps until input or a transport event (`Inbox::SetWake` → `WakeEventLoop`), at most 500 ms (100 ms while
  servers start or stop, for the stop timeouts), and draws a few frames after each event. ImGui's window state is saved in the
  user's preference folder (`SDL_GetPrefPath`).
- An unhandled exception is written to `crash.txt` in that folder and shown in a message box before the process aborts
  (`std::set_terminate`), instead of the C runtime's bare "abort() has been called".

## Tests

`x2_servermanager_tests` (`ctest -R servermanager`): recorded protocol lines, log store/filter/view, session with a fake transport,
and a session following a real `KServerConsole` in service mode over its control socket (hello, `ready`, a `START_LOG` with a field,
Lua results, a second client attaching late that gets the earlier lines in order and once each while a live one arrives, stop
until disconnection; the stop request is process-wide, so a second console cannot run in the same test process). Profile parsing, validation and order. Orchestrator with real children: three
`x2_terminal_demo` instances started in dependency order and stopped in reverse, a crash (`--exit-after`) that fails the server and
cancels its dependent, a missing program, a child that ignores stop (`x2_process_helper sleep`: terminated, then killed), and a
service already running that is attached through `x2ctl`, detached, attached again and stopped. Remote servers through
`x2_fake_ssh` (plays ssh: runs the remote command here, relaying the pipes): discovery, attach, detach, a stop of a listed server
that was not attached; an unreachable host and a server that is not running. The probe prints each host's discovery first.

`x2_servermanager_ui_tests` (`ctest -R servermanager_ui`) draws the window with ImGui's null backends, so ImGui's checks run over
its code: no profile and a profile that does not load; a failed local server and an unreachable host; a real `x2_terminal_demo`
started from it, a Lua command printing 300 lines (rows and clipper), and quitting (asks, then stops it, exit code 0). The window
itself (input, fonts, GPU) is tested by hand on a desktop. Socket directory:
`$XDG_RUNTIME_DIR/x2`. A failure prints each server's phase and last log entries.

`x2_servermanager_probe <dist | profile.json> [--attach-only]`: starts (or attaches to) every server, waits until all are ready,
stops them, and prints each phase change with its time.
