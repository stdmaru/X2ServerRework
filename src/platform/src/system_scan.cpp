#include "x2/platform/system_scan.h"

#if defined(_WIN32)
#include "x2/platform/types.h" // <windows.h>
#include <tlhelp32.h>
#else
#include "x2/platform/unicode.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#endif

namespace x2::platform {

#if defined(_WIN32)

std::vector<ProcessEntry> ListProcesses()
{
    std::vector<ProcessEntry> processes;
    const HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return processes;

    const DWORD self = ::GetCurrentProcessId();
    PROCESSENTRY32W entry = {};
    entry.dwSize = sizeof(entry);
    for (BOOL more = ::Process32FirstW(snapshot, &entry); more; more = ::Process32NextW(snapshot, &entry))
    {
        if (entry.th32ProcessID == self || entry.th32ProcessID == 0)
            continue;
        ProcessEntry process;
        process.pid = entry.th32ProcessID;
        process.path = entry.szExeFile;
        if (const HANDLE handle = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ProcessID))
        {
            wchar_t path[MAX_PATH] = {};
            DWORD size = MAX_PATH;
            if (::QueryFullProcessImageNameW(handle, 0, path, &size))
                process.path.assign(path, size);
            ::CloseHandle(handle);
        }
        processes.push_back(std::move(process));
    }
    ::CloseHandle(snapshot);
    return processes;
}

std::vector<DesktopWindow> ListTopLevelWindows()
{
    std::vector<DesktopWindow> windows;
    for (HWND hwnd = ::GetTopWindow(nullptr); hwnd != nullptr; hwnd = ::GetWindow(hwnd, GW_HWNDNEXT))
    {
        DesktopWindow window;
        wchar_t text[MAX_PATH] = {};
        if (const int length = ::GetWindowTextW(hwnd, text, MAX_PATH); length > 0)
            window.title.assign(text, static_cast<size_t>(length));
        if (const int length = ::GetClassNameW(hwnd, text, MAX_PATH); length > 0)
            window.className.assign(text, static_cast<size_t>(length));
        window.owned = ::GetParent(hwnd) != nullptr;
        windows.push_back(std::move(window));
    }
    return windows;
}

namespace {

bool Opened(HANDLE handle)
{
    if (handle != nullptr)
    {
        ::CloseHandle(handle);
        return true;
    }
    return ::GetLastError() == ERROR_ACCESS_DENIED;
}

} // namespace

bool NamedEventExists(const std::wstring& name)
{
    return !name.empty() && Opened(::OpenEventW(SYNCHRONIZE, FALSE, name.c_str()));
}

bool NamedMutexExists(const std::wstring& name)
{
    return !name.empty() && Opened(::OpenMutexW(SYNCHRONIZE, FALSE, name.c_str()));
}

#else

std::vector<ProcessEntry> ListProcesses()
{
    std::vector<ProcessEntry> processes;
    const long long self = ::getpid();
    std::error_code error;
    for (const auto& entry : std::filesystem::directory_iterator("/proc", error))
    {
        const std::string name = entry.path().filename().string();
        char* end = nullptr;
        const long long pid = std::strtoll(name.c_str(), &end, 10);
        if (name.empty() || *end != '\0' || pid == self)
            continue;

        ProcessEntry process;
        process.pid = pid;
        std::error_code linkError;
        const std::filesystem::path exe = std::filesystem::read_symlink(entry.path() / "exe", linkError);
        if (!linkError)
            process.path = Utf8ToWide(exe.string());
        else
        {
            // Another user's process: its executable is hidden, its name is not.
            std::ifstream comm(entry.path() / "comm");
            std::string command;
            if (!std::getline(comm, command))
                continue;
            process.path = Utf8ToWide(command);
        }
        processes.push_back(std::move(process));
    }
    return processes;
}

std::vector<DesktopWindow> ListTopLevelWindows() { return {}; }
bool NamedEventExists(const std::wstring&) { return false; }
bool NamedMutexExists(const std::wstring&) { return false; }

#endif

} // namespace x2::platform
