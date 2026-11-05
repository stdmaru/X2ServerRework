// Child processes with pipes (decision T32): ServerManager starts the servers with --machine and talks to them over
// stdin/stdout, and reaches remote machines through an `ssh` child. Text is UTF-8 on every platform.
#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace x2::platform {

struct ProcessOptions
{
    // Executable. A bare name is searched in PATH; a relative path with a directory part (./GameServer) is taken
    // relative to workingDirectory when that is set. On Windows, ".exe" may be left out.
    std::string program;
    std::vector<std::string> args;
    std::string workingDirectory;   // empty: the current one
    bool separateStderr = true;     // false: stderr goes to the stdout pipe
};

class Process
{
public:
    // Starts the child with its stdin, stdout and stderr connected to pipes. nullptr and `error` on failure
    // (including a program that does not exist). POSIX: the child gets its own process group, so a Ctrl+C in the
    // parent's terminal does not reach it, and the parent ignores SIGPIPE from then on (writes to a dead child fail).
    static std::unique_ptr<Process> Start(const ProcessOptions& options, std::string& error);

    // Kills the child if it is still running, and waits for it.
    ~Process();
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;

    long long Pid() const;

    // Thread-safe; false once the child closed its stdin (or exited).
    bool Write(std::string_view data);
    bool WriteLine(std::string_view line);  // adds "\n"
    void CloseStdin();

    // Block for the next line (without "\n" or "\r\n"); a last line without newline is returned too. False at the
    // end of the stream. Each stream may be read by its own thread.
    bool ReadLine(std::string& line);       // stdout (and stderr when not separate)
    bool ReadErrorLine(std::string& line);  // stderr; false at once when not separate

    // Asks the child to exit: SIGTERM on POSIX. Windows has no equivalent for a child without a console, so it closes
    // stdin (the servers exit at the end of stdin in --machine mode).
    void Terminate();
    void Kill();                            // SIGKILL / TerminateProcess

    // Exit code once the child exited (POSIX: 128 + signal when killed by a signal), or nullopt on timeout.
    std::optional<int> WaitFor(std::chrono::milliseconds timeout);
    int Wait();
    bool Running();

private:
    Process();
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

// Quoting for a command line read by the Microsoft C runtime (CommandLineToArgvW rules): used to build the child's
// command line on Windows.
std::string QuoteWindowsArgument(std::string_view arg);
// Quoting for a POSIX shell (single quotes): the remote command of `ssh host <command>` is run by the remote shell.
std::string QuoteShellArgument(std::string_view arg);

} // namespace x2::platform
