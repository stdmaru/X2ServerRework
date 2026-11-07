#include "x2/platform/process.h"

#if defined(_WIN32)
#include "x2/platform/types.h" // <windows.h>
#include "x2/platform/unicode.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <shellapi.h>
#else
#include <cerrno>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include "x2/platform/unicode.h"
#endif

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <mutex>
#include <thread>

#if !defined(_WIN32)
extern char** environ;
#endif

namespace x2::platform {

long long CurrentProcessId()
{
#if defined(_WIN32)
    return ::GetCurrentProcessId();
#else
    return ::getpid();
#endif
}

long long ParentProcessId()
{
#if defined(_WIN32)
    const DWORD self = ::GetCurrentProcessId();
    const HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;
    long long parent = 0;
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    for (BOOL more = ::Process32FirstW(snapshot, &entry); more; more = ::Process32NextW(snapshot, &entry))
        if (entry.th32ProcessID == self) {
            parent = entry.th32ParentProcessID;
            break;
        }
    ::CloseHandle(snapshot);
    return parent;
#else
    return ::getppid();
#endif
}

bool CurrentProcessMemory(std::size_t& current, std::size_t& peak)
{
    current = peak = 0;
#if defined(_WIN32)
    PROCESS_MEMORY_COUNTERS counters{};
    if (!::GetProcessMemoryInfo(::GetCurrentProcess(), &counters, sizeof(counters)))
        return false;
    current = counters.WorkingSetSize;
    peak = counters.PeakWorkingSetSize;
    return true;
#else
    // VmRSS and VmHWM (the resident set's high-water mark) of /proc/self/status, in kB
    std::FILE* status = std::fopen("/proc/self/status", "r");
    if (!status)
        return false;
    char line[256];
    int found = 0;
    while (std::fgets(line, sizeof(line), status)) {
        unsigned long long kb = 0;
        if (std::sscanf(line, "VmRSS: %llu kB", &kb) == 1) {
            current = static_cast<std::size_t>(kb) * 1024;
            ++found;
        } else if (std::sscanf(line, "VmHWM: %llu kB", &kb) == 1) {
            peak = static_cast<std::size_t>(kb) * 1024;
            ++found;
        }
    }
    std::fclose(status);
    return found == 2;
#endif
}

bool PhysicalMemory(std::size_t& total, std::size_t& available)
{
    total = available = 0;
#if defined(_WIN32)
    MEMORYSTATUSEX status{};
    status.dwLength = sizeof(status);
    if (!::GlobalMemoryStatusEx(&status))
        return false;
    total = static_cast<std::size_t>(status.ullTotalPhys);
    available = static_cast<std::size_t>(status.ullAvailPhys);
    return true;
#else
    // MemTotal and MemAvailable of /proc/meminfo, in kB
    std::FILE* info = std::fopen("/proc/meminfo", "r");
    if (!info)
        return false;
    char line[256];
    int found = 0;
    while (std::fgets(line, sizeof(line), info)) {
        unsigned long long kb = 0;
        if (std::sscanf(line, "MemTotal: %llu kB", &kb) == 1) {
            total = static_cast<std::size_t>(kb) * 1024;
            ++found;
        } else if (std::sscanf(line, "MemAvailable: %llu kB", &kb) == 1) {
            available = static_cast<std::size_t>(kb) * 1024;
            ++found;
        }
    }
    std::fclose(info);
    return found == 2;
#endif
}

void SetEnvironment(const std::string& name, const std::string& value)
{
#if defined(_WIN32)
    // the C runtime's copy and the process's (which CreateProcess passes on)
    ::_wputenv_s(Utf8ToWide(name).c_str(), Utf8ToWide(value).c_str());
#else
    if (value.empty())
        ::unsetenv(name.c_str());
    else
        ::setenv(name.c_str(), value.c_str(), 1);
#endif
}

std::string QuoteWindowsArgument(std::string_view arg)
{
    if (!arg.empty() && arg.find_first_of(" \t\n\v\"") == std::string_view::npos)
        return std::string(arg);
    std::string out = "\"";
    std::size_t backslashes = 0;
    for (const char c : arg) {
        if (c == '\\') {
            ++backslashes;
            continue;
        }
        if (c == '"')
            out.append(backslashes * 2 + 1, '\\');      // escape the backslashes and the quote
        else
            out.append(backslashes, '\\');
        backslashes = 0;
        out += c;
    }
    out.append(backslashes * 2, '\\');                  // before the closing quote
    out += '"';
    return out;
}

std::string QuoteCmdArgument(std::string_view arg)
{
    // cmd.exe reads the line first: a caret makes the next character literal, quotes included, so no quote state hides
    // a metacharacter from it, and ^% breaks %VAR% expansion. The program then sees QuoteWindowsArgument's quoting.
    std::string out;
    for (const char c : QuoteWindowsArgument(arg)) {
        if (std::string_view("\"^&|<>()%!").find(c) != std::string_view::npos)
            out += '^';
        out += c;
    }
    return out;
}

std::string QuoteShellArgument(std::string_view arg)
{
    if (!arg.empty() && arg.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-./=:,+@%")
                            == std::string_view::npos)
        return std::string(arg);
    std::string out = "'";
    for (const char c : arg) {
        if (c == '\'')
            out += "'\\''";
        else
            out += c;
    }
    out += '\'';
    return out;
}

namespace {

// ./GameServer with a working directory: relative to it, on every platform.
std::string ResolveProgram(const ProcessOptions& options)
{
    const std::filesystem::path program(std::u8string_view(reinterpret_cast<const char8_t*>(options.program.data()),
                                                           options.program.size()));
    if (options.workingDirectory.empty() || program.is_absolute() || !program.has_parent_path())
        return options.program;
    const std::filesystem::path dir(std::u8string_view(reinterpret_cast<const char8_t*>(options.workingDirectory.data()),
                                                       options.workingDirectory.size()));
    const std::u8string joined = (dir / program).lexically_normal().u8string();
    return std::string(joined.begin(), joined.end());
}

// Splits a byte stream into lines.
class LineReader
{
public:
    template <class ReadSome>
    bool Next(std::string& line, ReadSome readSome)
    {
        for (;;) {
            const std::size_t nl = m_buffer.find('\n', m_scanned);
            if (nl != std::string::npos) {
                line.assign(m_buffer, 0, nl);
                if (!line.empty() && line.back() == '\r')
                    line.pop_back();
                m_buffer.erase(0, nl + 1);
                m_scanned = 0;
                return true;
            }
            m_scanned = m_buffer.size();
            if (m_eof)
                break;
            char chunk[4096];
            const long n = readSome(chunk, sizeof(chunk));
            if (n <= 0)
                m_eof = true;
            else
                m_buffer.append(chunk, static_cast<std::size_t>(n));
        }
        if (m_buffer.empty())
            return false;
        line.swap(m_buffer);
        m_buffer.clear();
        m_scanned = 0;
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        return true;
    }

private:
    std::string m_buffer;
    std::size_t m_scanned = 0;
    bool m_eof = false;
};

} // namespace

#if defined(_WIN32)

struct Process::Impl
{
    HANDLE process = nullptr;
    DWORD pid = 0;
    std::mutex writeMutex;
    HANDLE in = nullptr;
    HANDLE out = nullptr;
    HANDLE err = nullptr;
    LineReader outLines, errLines;

    ~Impl()
    {
        for (HANDLE h : {in, out, err, process})
            if (h)
                ::CloseHandle(h);
    }

    static long ReadSome(HANDLE h, char* buffer, std::size_t size)
    {
        DWORD n = 0;
        if (!h || !::ReadFile(h, buffer, static_cast<DWORD>(size), &n, nullptr))
            return 0;   // ERROR_BROKEN_PIPE: the child closed it
        return static_cast<long>(n);
    }
};

namespace {

std::string LastErrorMessage(const char* what)
{
    const DWORD code = ::GetLastError();
    wchar_t* text = nullptr;
    ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
                     code, 0, reinterpret_cast<wchar_t*>(&text), 0, nullptr);
    std::string message = text ? WideToUtf8(text) : std::string();
    if (text)
        ::LocalFree(text);
    while (!message.empty() && (message.back() == '\n' || message.back() == '\r' || message.back() == ' '))
        message.pop_back();
    return std::string(what) + ": " + message + " (" + std::to_string(code) + ")";
}

// Pipe whose child end is inheritable and whose parent end is not.
bool ChildPipe(HANDLE& parent, HANDLE& child, bool childReads)
{
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    HANDLE readEnd = nullptr, writeEnd = nullptr;
    if (!::CreatePipe(&readEnd, &writeEnd, &sa, 0))
        return false;
    parent = childReads ? writeEnd : readEnd;
    child = childReads ? readEnd : writeEnd;
    ::SetHandleInformation(parent, HANDLE_FLAG_INHERIT, 0);
    return true;
}

} // namespace

Process::Process() : m_impl(std::make_unique<Impl>()) {}

std::unique_ptr<Process> Process::Start(const ProcessOptions& options, std::string& error)
{
    std::unique_ptr<Process> self(new Process);
    Impl& d = *self->m_impl;

    HANDLE childIn = nullptr, childOut = nullptr, childErr = nullptr;
    auto closeChildEnds = [&] {
        for (HANDLE h : {childIn, childOut, childErr})
            if (h)
                ::CloseHandle(h);
    };
    if (!ChildPipe(d.in, childIn, true) || !ChildPipe(d.out, childOut, false) ||
        (options.separateStderr && !ChildPipe(d.err, childErr, false))) {
        error = LastErrorMessage("CreatePipe");
        closeChildEnds();
        return nullptr;
    }

    // Only the three pipe ends are inherited (not every inheritable handle of this process).
    HANDLE inherit[3] = {childIn, childOut, childErr ? childErr : nullptr};
    const DWORD inheritCount = childErr ? 3 : 2;
    SIZE_T attrSize = 0;
    ::InitializeProcThreadAttributeList(nullptr, 1, 0, &attrSize);
    std::vector<char> attrBuffer(attrSize);
    auto* attrs = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(attrBuffer.data());
    if (!::InitializeProcThreadAttributeList(attrs, 1, 0, &attrSize) ||
        !::UpdateProcThreadAttribute(attrs, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST, inherit, inheritCount * sizeof(HANDLE),
                                     nullptr, nullptr)) {
        error = LastErrorMessage("UpdateProcThreadAttribute");
        closeChildEnds();
        return nullptr;
    }

    STARTUPINFOEXW si{};
    si.StartupInfo.cb = sizeof(si);
    si.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    si.StartupInfo.hStdInput = childIn;
    si.StartupInfo.hStdOutput = childOut;
    si.StartupInfo.hStdError = childErr ? childErr : childOut;
    si.lpAttributeList = attrs;

    std::string commandLine = QuoteWindowsArgument(ResolveProgram(options));
    for (const std::string& arg : options.args)
        commandLine += ' ' + QuoteWindowsArgument(arg);
    std::wstring wideCommand = Utf8ToWide(commandLine);
    const std::wstring wideDir = Utf8ToWide(options.workingDirectory);

    PROCESS_INFORMATION pi{};
    const BOOL ok = ::CreateProcessW(nullptr, wideCommand.data(), nullptr, nullptr, TRUE,
                                     CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT | EXTENDED_STARTUPINFO_PRESENT,
                                     nullptr, wideDir.empty() ? nullptr : wideDir.c_str(), &si.StartupInfo, &pi);
    if (!ok)
        error = LastErrorMessage("CreateProcess");
    ::DeleteProcThreadAttributeList(attrs);
    closeChildEnds();
    if (!ok)
        return nullptr;

    ::CloseHandle(pi.hThread);
    d.process = pi.hProcess;
    d.pid = pi.dwProcessId;
    return self;
}

Process::~Process()
{
    if (m_impl->process && Running()) {
        Kill();
        Wait();
    }
}

long long Process::Pid() const
{
    return m_impl->pid;
}

bool Process::Write(std::string_view data)
{
    std::lock_guard lock(m_impl->writeMutex);
    while (!data.empty()) {
        DWORD n = 0;
        if (!m_impl->in || !::WriteFile(m_impl->in, data.data(), static_cast<DWORD>(data.size()), &n, nullptr))
            return false;
        data.remove_prefix(n);
    }
    return true;
}

void Process::CloseStdin()
{
    std::lock_guard lock(m_impl->writeMutex);
    if (m_impl->in) {
        ::CloseHandle(m_impl->in);
        m_impl->in = nullptr;
    }
}

bool Process::ReadLine(std::string& line)
{
    HANDLE h = m_impl->out;
    return m_impl->outLines.Next(line, [h](char* b, std::size_t n) { return Impl::ReadSome(h, b, n); });
}

bool Process::ReadErrorLine(std::string& line)
{
    HANDLE h = m_impl->err;
    if (!h)
        return false;
    return m_impl->errLines.Next(line, [h](char* b, std::size_t n) { return Impl::ReadSome(h, b, n); });
}

void Process::Terminate()
{
    CloseStdin();
}

void Process::Kill()
{
    ::TerminateProcess(m_impl->process, 1);
}

std::optional<int> Process::WaitFor(std::chrono::milliseconds timeout)
{
    const DWORD ms = timeout.count() < 0 ? INFINITE : static_cast<DWORD>(timeout.count());
    if (::WaitForSingleObject(m_impl->process, ms) != WAIT_OBJECT_0)
        return std::nullopt;
    DWORD code = 0;
    ::GetExitCodeProcess(m_impl->process, &code);
    return static_cast<int>(code);
}

int Process::Wait()
{
    return *WaitFor(std::chrono::milliseconds(-1));
}

bool Process::Running()
{
    return !WaitFor(std::chrono::milliseconds(0));
}

#else // POSIX

struct Process::Impl
{
    pid_t pid = -1;
    std::mutex writeMutex;
    int in = -1;
    int out = -1;
    int err = -1;
    LineReader outLines, errLines;

    std::mutex waitMutex;   // guards the reaping, so Kill never signals a reused pid
    bool exited = false;
    int exitCode = 0;

    ~Impl()
    {
        for (int fd : {in, out, err})
            if (fd >= 0)
                ::close(fd);
    }

    static long ReadSome(int fd, char* buffer, std::size_t size)
    {
        for (;;) {
            const ssize_t n = ::read(fd, buffer, size);
            if (n < 0 && errno == EINTR)
                continue;
            return static_cast<long>(n);
        }
    }

    // With waitMutex held.
    bool Reap(int options)
    {
        if (exited)
            return true;
        int status = 0;
        pid_t r;
        do {
            r = ::waitpid(pid, &status, options);
        } while (r < 0 && errno == EINTR);
        if (r != pid)
            return false;
        exited = true;
        exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : WIFSIGNALED(status) ? 128 + WTERMSIG(status) : 0;
        return true;
    }
};

Process::Process() : m_impl(std::make_unique<Impl>()) {}

std::unique_ptr<Process> Process::Start(const ProcessOptions& options, std::string& error)
{
    static std::once_flag ignoreSigpipe;
    std::call_once(ignoreSigpipe, [] { std::signal(SIGPIPE, SIG_IGN); });

    std::unique_ptr<Process> self(new Process);
    Impl& d = *self->m_impl;

    int inPipe[2] = {-1, -1}, outPipe[2] = {-1, -1}, errPipe[2] = {-1, -1};
    auto closeAll = [&] {
        for (int fd : {inPipe[0], inPipe[1], outPipe[0], outPipe[1], errPipe[0], errPipe[1]})
            if (fd >= 0)
                ::close(fd);
    };
    if (::pipe2(inPipe, O_CLOEXEC) != 0 || ::pipe2(outPipe, O_CLOEXEC) != 0 ||
        (options.separateStderr && ::pipe2(errPipe, O_CLOEXEC) != 0)) {
        error = std::string("pipe: ") + std::strerror(errno);
        closeAll();
        return nullptr;
    }

    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);
    posix_spawn_file_actions_adddup2(&actions, inPipe[0], 0);       // dup2 clears close-on-exec
    posix_spawn_file_actions_adddup2(&actions, outPipe[1], 1);
    posix_spawn_file_actions_adddup2(&actions, options.separateStderr ? errPipe[1] : outPipe[1], 2);
    if (!options.workingDirectory.empty())
        posix_spawn_file_actions_addchdir_np(&actions, options.workingDirectory.c_str());

    posix_spawnattr_t attr;
    posix_spawnattr_init(&attr);
    sigset_t defaults, empty;
    sigemptyset(&defaults);
    for (int sig : {SIGPIPE, SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGCHLD})
        sigaddset(&defaults, sig);
    sigemptyset(&empty);
    posix_spawnattr_setsigdefault(&attr, &defaults);
    posix_spawnattr_setsigmask(&attr, &empty);
    posix_spawnattr_setpgroup(&attr, 0);
    posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSIGDEF | POSIX_SPAWN_SETSIGMASK | POSIX_SPAWN_SETPGROUP);

    const std::string program = ResolveProgram(options);
    std::vector<char*> argv;
    argv.push_back(const_cast<char*>(program.c_str()));
    for (const std::string& arg : options.args)
        argv.push_back(const_cast<char*>(arg.c_str()));
    argv.push_back(nullptr);

    pid_t pid = -1;
    const int rc = ::posix_spawnp(&pid, program.c_str(), &actions, &attr, argv.data(), environ);
    posix_spawn_file_actions_destroy(&actions);
    posix_spawnattr_destroy(&attr);
    if (rc != 0) {
        error = program + ": " + std::strerror(rc);
        closeAll();
        return nullptr;
    }

    d.pid = pid;
    d.in = inPipe[1];
    d.out = outPipe[0];
    d.err = errPipe[0];
    for (int fd : {inPipe[0], outPipe[1], errPipe[1]})     // the child's ends
        if (fd >= 0)
            ::close(fd);
    return self;
}

Process::~Process()
{
    if (m_impl->pid > 0 && Running()) {
        Kill();
        Wait();
    }
}

long long Process::Pid() const
{
    return m_impl->pid;
}

bool Process::Write(std::string_view data)
{
    std::lock_guard lock(m_impl->writeMutex);
    while (!data.empty()) {
        if (m_impl->in < 0)
            return false;
        const ssize_t n = ::write(m_impl->in, data.data(), data.size());
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0)
            return false;   // EPIPE: the child closed it (SIGPIPE is ignored)
        data.remove_prefix(static_cast<std::size_t>(n));
    }
    return true;
}

void Process::CloseStdin()
{
    std::lock_guard lock(m_impl->writeMutex);
    if (m_impl->in >= 0) {
        ::close(m_impl->in);
        m_impl->in = -1;
    }
}

bool Process::ReadLine(std::string& line)
{
    const int fd = m_impl->out;
    return m_impl->outLines.Next(line, [fd](char* b, std::size_t n) { return Impl::ReadSome(fd, b, n); });
}

bool Process::ReadErrorLine(std::string& line)
{
    const int fd = m_impl->err;
    if (fd < 0)
        return false;
    return m_impl->errLines.Next(line, [fd](char* b, std::size_t n) { return Impl::ReadSome(fd, b, n); });
}

void Process::Terminate()
{
    std::lock_guard lock(m_impl->waitMutex);
    if (!m_impl->Reap(WNOHANG))
        ::kill(m_impl->pid, SIGTERM);
}

void Process::Kill()
{
    std::lock_guard lock(m_impl->waitMutex);
    if (!m_impl->Reap(WNOHANG))
        ::kill(m_impl->pid, SIGKILL);
}

std::optional<int> Process::WaitFor(std::chrono::milliseconds timeout)
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    for (;;) {
        {
            std::lock_guard lock(m_impl->waitMutex);
            if (m_impl->Reap(WNOHANG))
                return m_impl->exitCode;
        }
        if (timeout.count() >= 0 && std::chrono::steady_clock::now() >= deadline)
            return std::nullopt;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

int Process::Wait()
{
    return *WaitFor(std::chrono::milliseconds(-1));
}

bool Process::Running()
{
    return !WaitFor(std::chrono::milliseconds(0));
}

#endif

#if defined(_WIN32)

bool OpenUrl(const std::wstring& url)
{
    const auto result = reinterpret_cast<INT_PTR>(
        ::ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL));
    return result > 32;
}

#else

bool OpenUrl(const std::wstring& url)
{
    const std::string command = "xdg-open " + QuoteShellArgument(WideToUtf8(url)) + " >/dev/null 2>&1 &";
    return std::system(command.c_str()) == 0;
}


bool Process::WriteLine(std::string_view line)
{
    std::string data(line);
    data += '\n';
    return Write(data);
}

std::string MachineIdentity()
{
#if defined(_WIN32)
    char value[128] = {};
    DWORD size = sizeof(value);
    if (RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", "MachineGuid",
                     RRF_RT_REG_SZ | RRF_SUBKEY_WOW6464KEY, nullptr, value, &size) != ERROR_SUCCESS)
        return {};
    return value;
#else
    for (const char* path : {"/etc/machine-id", "/var/lib/dbus/machine-id"}) {
        if (FILE* file = std::fopen(path, "r")) {
            char value[128] = {};
            const bool read = std::fgets(value, sizeof(value), file) != nullptr;
            std::fclose(file);
            std::string id = read ? value : "";
            while (!id.empty() && (id.back() == '\n' || id.back() == '\r'))
                id.pop_back();
            if (!id.empty())
                return id;
        }
    }
    return {};
#endif
}

} // namespace x2::platform
