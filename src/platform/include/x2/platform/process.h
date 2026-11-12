// Child processes with pipes (decision T32): ServerManager starts the servers with --machine and talks to them over
// stdin/stdout, and reaches remote machines through an `ssh` child. Text is UTF-8 on every platform.
#pragma once

#include <chrono>
#include <cstddef>
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

long long CurrentProcessId();
// The process that started this one (ssh, for ServerManager's askpass); 0 when unknown.
long long ParentProcessId();

// This process's memory in bytes: resident (working set) now and its peak. False when the system does not say.
bool CurrentProcessMemory(std::size_t& current, std::size_t& peak);
// The machine's physical memory in bytes: installed (as the system sees it) and available now. False when unknown.
bool PhysicalMemory(std::size_t& total, std::size_t& available);

// A string that identifies this machine and stays the same across reboots: the MachineGuid on Windows, the
// machine-id (systemd / D-Bus) elsewhere. Empty when the system has none.
std::string MachineIdentity();

// Sets (or, with an empty value, removes) a variable of this process's environment, which the children started from
// then on inherit. Not thread-safe with other environment reads (getenv): call it before starting threads.
void SetEnvironment(const std::string& name, const std::string& value);

// Quoting for a command line read by the Microsoft C runtime (CommandLineToArgvW rules): used to build the child's
// command line on Windows.
std::string QuoteWindowsArgument(std::string_view arg);
// Quoting for one argument of a cmd.exe command line (`cmd /c`, the default shell of Windows' OpenSSH): the program
// receives it unchanged. Not for the program itself, which cmd reads before the carets: QuoteWindowsArgument.
std::string QuoteCmdArgument(std::string_view arg);
// Quoting for a POSIX shell (single quotes): the remote command of `ssh host <command>` is run by the remote shell.
std::string QuoteShellArgument(std::string_view arg);

// A lock held by one process of the user at a time, by name (the client's "ElswordX2" mutex): a named mutex on
// Windows, an exclusive flock on <runtime dir>/<name>.lock elsewhere ($XDG_RUNTIME_DIR, else the temp directory).
// Released when destroyed or when the process ends.
class SingleInstance
{
public:
    // nullptr when another process holds it (or it could not be created).
    static std::unique_ptr<SingleInstance> Acquire(const std::string& name);
    ~SingleInstance();
    SingleInstance(const SingleInstance&) = delete;
    SingleInstance& operator=(const SingleInstance&) = delete;

private:
    SingleInstance() = default;
#if defined(_WIN32)
    void* m_mutex = nullptr;
#else
    int m_fd = -1;
#endif
};

// Opens a URL in the user's browser without waiting for it (ShellExecute "open" on Windows, xdg-open elsewhere).
// False when it could not be started.
bool OpenUrl(const std::wstring& url);

} // namespace x2::platform
