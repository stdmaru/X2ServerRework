[Home](../README.md) › [Porting](README.md) › Server console

# Server console (`KServerConsole`, `x2ctl`)

This replaces the original MFC window (`Common/ui/ServerUIDlg`, see [Server](../original/server.md)): the terminal from decision T15
and control protocol from T30, forming the foundation for ServerManager. Code: `src/kncsdk/ServerConsole.{h,cpp}` (`KServerConsole`),
on top of `x2::platform::Terminal` and `x2::platform::ControlServer` ([Platform](platform.md)); `src/server/x2ctl`. Implemented on
2026-10-31 (`server/terminal`, `platform/terminal-windows`, `server/control`).

`KServerMain` opens the console before `Init` (the original window was present during loading), using the executable name as its title
(the window was called `GameServer`...), and closes it after `ShutDown`. EPublisherServer uses the same console and options.

## Modes

| Mode | When | Behavior |
|---|---|---|
| Terminal | stdin and stdout are terminals | Raw input and line editor (←/→, Home/End, Ctrl+A/E/U/K/W, Ctrl+←/→, ↑/↓ history, Ctrl+L clears screen). Logs (`SetConsoleWriter`) and all stdout/stderr output (a pipe, like the original `CHookStdio`) appear above the prompt, which is redrawn. Title `<name>_Loading`, then `_Ready` (checked every ~100 ms, like the original 5 ms loop). Ctrl+C clears the line; with an empty line, Ctrl+C or Ctrl+D asks `Are you sure you want to exit? [y/N]` (original text) |
| Lines | stdin is a pipe or file | Each line is a Lua command, with no prompt or echo; end of stdin exits |
| Machine | `--machine` | stdin/stdout use the [protocol](#protocol): stdout contains only JSON (human-readable logging is disabled and loose stdout/stderr become `output` messages); end of stdin exits |
| Service | `--no-stdin` | Reads nothing; waits for SIGTERM/SIGINT/SIGHUP (Windows: Ctrl+C, console close, logoff, shutdown) |

In all modes the server also listens on the **control socket** (below), unless `--no-control` is specified. Console options are not passed
to the server as arguments: `--no-stdin`, `--machine`, `--control=<name>` (socket name; defaults to executable name), and `--no-control`.

- Lua-like `lua.c` command behavior: an expression prints its value (`1+1` → `2`), otherwise it runs as a statement; errors appear in red in terminal mode.
  The original called `luaL_dostring` and discarded errors.
- Server code must not write to `std::wcout`/`std::wcerr`. With stdio synchronization (the default), the first write fixes `stdout` as
  wide-oriented on glibc, and every narrow write after it (Lua `print`/`io.write`, `printf`, `std::cout`) fails silently. The original
  used `std::wcout` for `Dump()` (`DeclDump`), `Server:SetUseVersion` (called by `config_gs*.lua`), and `KGameServer::ServerVersion`, so
  the GameServer console printed nothing (fixed on 2026-11-04). Wide text goes through `KWriteStdout`/`KDumpToStdout`
  (`src/kncsdk/ToString.h`) as UTF-8; the regression test is in `test_util.cpp`.
- Lines (lines and machine modes) are read from the stdin handle (`read`/`ReadFile`), not through `std::cin`: a thread blocked in
  `std::getline` holds the stdin `FILE` lock, which `exit()` waits for when it flushes the streams, so a `--machine` server stopped
  with `{"stop":true}` finished `ShutDown` but only exited once its stdin was closed (found by ServerManager on 2026-11-05).
- Keys typed during loading wait for the prompt; Ctrl+C during that time asks for confirmation as soon as the prompt appears.
- SIGTERM and console close exit without asking. On Windows, console close allows ~5 seconds for `ShutDown` (system limit).
- In a terminal, a fatal failure (SIGSEGV, SIGABRT...) restores terminal mode before exit, so the shell is not left in raw mode.
- Linux: `termios` without `ECHO`/`ICANON`/`ISIG` (output processing remains enabled). Windows: `ReadConsoleW` with `ENABLE_VIRTUAL_TERMINAL_INPUT`
  (keys arrive as text and VT sequences, using the same decoder as Linux), because UTF-8 from conhost stdin is unreliable.
- No line-editing library (T13): the options (readline, linenoise, replxx) do not cover the Windows console or bring more than needed;
  the editor is small and tested (`test_terminal.cpp`).
- `x2_terminal_demo` (`src/kncsdk/tests/terminal_demo.cpp`, built with the tests): same console without server or database (socket
  `TerminalDemo`), one log line every 2 seconds, and ready state after 3 seconds, for manual testing. Tested on Linux and Windows.
  For the tests: `--control=<name>` (socket and name), `--ready-after=<ms>`, `--exit-after=<ms>` (abrupt exit with code 5), and
  `--interpreter` (runs `lua_interpreter`).

## Control socket

A Unix socket (`AF_UNIX`, available on Windows since version 1803) in a user-only directory, so the same implementation works on
both platforms instead of using *named pipes* on Windows:
`$XDG_RUNTIME_DIR/x2/<name>.sock` (or `/tmp/x2-<uid>/`, mode 0700) on Linux, `%TEMP%\x2\<name>.sock` on Windows. It speaks the same
[protocol](#protocol) to any number of clients; all receive logs and status, while a command result goes only to its requester.

- A second server with the same name cannot open the socket (warning logged) and continues without it; use `--control=<other>`. A
  socket file with no process behind it (a crashed process) is replaced.
- Each client has a queue and writer thread; a client with more than 8 MB waiting in its queue is disconnected, so it can never block logging.
- Authentication and encryption are handled by SSH (T30): the socket accepts only the local user.
- Service mode does not redirect stdout, so attached clients do not receive `output` messages (only logs).
- The server side never blocks in `accept`/`recv`/`send`: waits with `poll` (`WSAPoll`) for 100 ms and a stop signal, with nonblocking
  client sockets. On Windows, closing an `AF_UNIX` socket from another thread does not wake a thread blocked on it, which caused `Stop`
  to hang (found in the VM on 2026-10-31). `Stop` waits up to 2 seconds for each client's queue to drain (final `status`).
- On Windows, the socket file is a *reparse point* that `std::filesystem` cannot inspect (`exists` throws): a leftover from a crashed
  process is detected by attempting to connect.

## `x2ctl`

Built with the servers and copied to `dist/x2ctl`. Tested on Linux and Windows (2026-10-31); on Windows, arguments are read as UTF-16
(`x2::platform::Utf8Arguments`), otherwise Korean text passed to `x2ctl run` arrived as `?`.

| Command | Action |
|---|---|
| `x2ctl list` | list names of listening servers |
| `x2ctl attach <name>` | connect stdin/stdout to the socket (protocol lines): `ssh host x2ctl attach GameServer` is how ServerManager will talk to a remote server |
| `x2ctl run <name> <lua...>` | run one command; print its output and errors to stderr; exit code 1 if Lua failed |
| `x2ctl stop <name>` | stop the server and wait for it to exit |

## Protocol

JSON, one object per line (UTF-8), shared by `--machine` and the socket. Version 1 (`hello` contains `protocol`).

From server:

| `type` | Fields | When |
|---|---|---|
| `hello` | `protocol`, `server` (name), `pid`, `state`, `seq` (last line number sent before it, 0 if none) | first line (on socket, before anything else) |
| `log` | `time` (`YYYY-MM-DD hh:mm:ss`), `level` (`error`, `warn`, `info`, `note`), `stream` (`cerr`, `cout2`...), `func`, `file`, `line`, `message`, `fields` (`[{"name","value"}]`, from `BUILD_LOG`), `text` (other lines) | each [log](kncsdk.md#logging-t16) at console level (`consolelog( n )` changes it) |
| `output` | `text` | a line written to stdout/stderr outside logging (`std::cout`, `printf`) |

`log` and `output` lines also carry `seq`, a line number shared by both (from 1), for the [history](#history).
| `status` | `state`: `ready`, `stopping` (`ShutDown` begins), `stopped` (console closes) | state changes |
| `result` | `id` (request ID or `null`), `ok`, `output` (what the command printed with `print`), `error` | command response |

From client: `{"id":1,"lua":"print(1)"}` (`id` may be any number or string and is returned in `result`), `{"id":2,"stop":true}` (stop),
`{"id":3,"history":true,"upto":120}` ([history](#history)), or a raw Lua line (response has `id` `null`). Only `print` is captured in the response; other command output (`io.write`, `os.execute`)
is emitted as `output`. Socket commands run on the prompt thread (owner of Lua state), between key events.

### History

Added on 2026-11-05 (`server/control-history`), compatible with version 1 (new fields and one new request; no new port). The
console keeps its latest `log` and `output` lines (at most 10,000 lines and 4 MB, below the 8 MB a client may have queued) with their
`seq`. `{"id":n,"history":true,"upto":S}` sends the kept lines with `seq` <= S (all of them without `upto`), as they were sent, then
`{"type":"result","id":n,"ok":true,...}`. It is answered at once on the client's reader thread (or the machine stdin), without
waiting for the Lua thread, so it works while the server loads.

A client asks with `upto` = the `seq` of its `hello`. Lines are numbered and sent under one lock, and the control server makes the
greeting and registers the client in one step, so every line after the greeting's `seq` reaches the client live and every line
up to it is in the backlog (unless dropped as too old). Live lines may arrive between the replayed ones, and a line numbered just
as the client connected may come both ways: the client merges by `seq` ([ServerManager](../tools/servermanager.md)). A raw
`x2ctl attach` sees the same lines.

Example (`x2ctl attach ChannelServer`, shortened):

```json
{"type":"hello","protocol":1,"server":"ChannelServer","pid":577544,"state":"loading","seq":0}
{"seq":1,"type":"log","time":"2026-10-31 20:21:31","level":"note","stream":"cout","func":"SetPacketAuthFailLimit_LUA","file":".../NetLayer.cpp","line":181,"message":"Packet Auth Fail Limit : 100","fields":[],"text":[]}
{"type":"status","state":"ready"}
{"id":1,"lua":"GetLocalIP()"}
{"type":"result","id":1,"ok":true,"output":"127.0.0.1\n","error":""}
```
