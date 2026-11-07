#include "x2/platform/console.h"

#if defined(_WIN32)
#include "x2/platform/types.h" // <windows.h>
#include "x2/platform/unicode.h"
#include <io.h>
#include <shellapi.h>
#else
#include <unistd.h>
#endif

#include <cstdlib>

namespace x2::platform {

#if defined(_WIN32)

namespace {

UINT g_inputCodePage = 0;
UINT g_outputCodePage = 0;

void RestoreCodePages()
{
    if (g_inputCodePage != 0)
        ::SetConsoleCP(g_inputCodePage);
    if (g_outputCodePage != 0)
        ::SetConsoleOutputCP(g_outputCodePage);
}

void EnableEscapes(DWORD handleId)
{
    const HANDLE handle = ::GetStdHandle(handleId);
    DWORD mode = 0;
    if (handle != INVALID_HANDLE_VALUE && ::GetConsoleMode(handle, &mode))
        ::SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

} // namespace

void InitConsole()
{
    static bool done = false;
    if (done)
        return;
    done = true;

    // Without a console (redirected output, service) these fail and the bytes go out as UTF-8 anyway.
    g_inputCodePage = ::GetConsoleCP();
    g_outputCodePage = ::GetConsoleOutputCP();
    if (g_inputCodePage != 0 || g_outputCodePage != 0)
        std::atexit(RestoreCodePages);
    ::SetConsoleCP(CP_UTF8);
    ::SetConsoleOutputCP(CP_UTF8);

    EnableEscapes(STD_OUTPUT_HANDLE);
    EnableEscapes(STD_ERROR_HANDLE);
}

bool IsColorTerminal(std::FILE* stream)
{
    const int fd = ::_fileno(stream);
    if (fd < 0 || !::_isatty(fd))
        return false;
    DWORD mode = 0;
    const HANDLE handle = reinterpret_cast<HANDLE>(::_get_osfhandle(fd));
    return ::GetConsoleMode(handle, &mode) && (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
}

std::vector<std::string> Utf8Arguments(int argc, char* argv[])
{
    int count = 0;
    LPWSTR* wide = ::CommandLineToArgvW(::GetCommandLineW(), &count);
    if (!wide || count != argc) {
        if (wide)
            ::LocalFree(wide);
        return std::vector<std::string>(argv, argv + argc);
    }
    std::vector<std::string> args;
    for (int i = 0; i < count; ++i)
        args.push_back(WideToUtf8(wide[i]));
    ::LocalFree(wide);
    return args;
}

void OpenDebugConsole()
{
    if (::GetConsoleWindow() != nullptr || !::AllocConsole())
        return;
    std::FILE* stream = nullptr;
    ::freopen_s(&stream, "CONOUT$", "a", stdout);
    ::freopen_s(&stream, "CONOUT$", "a", stderr);
}

void CloseDebugConsole()
{
    ::FreeConsole();
}

#else

void InitConsole()
{
}

std::vector<std::string> Utf8Arguments(int argc, char* argv[])
{
    return std::vector<std::string>(argv, argv + argc);
}

bool IsColorTerminal(std::FILE* stream)
{
    const int fd = ::fileno(stream);
    return fd >= 0 && ::isatty(fd) != 0;
}

void OpenDebugConsole()
{
}

void CloseDebugConsole()
{
}

#endif

} // namespace x2::platform
