#include "x2/platform/debug.h"

#include <cstdlib>
#include <fstream>
#include <string>
#include "x2/platform/unicode.h"

#include <cstdio>
#include <mutex>
#include <string>
#include <unordered_map>

namespace x2::platform {

const wchar_t* WidenStatic(const char* name)
{
    static std::mutex mutex;
    static std::unordered_map<const char*, std::wstring> cache; // keyed by address: callers pass static strings
    std::lock_guard lock(mutex);
    auto [it, inserted] = cache.try_emplace(name);
    if (inserted)
        it->second = Utf8ToWide(name ? name : "");
    return it->second.c_str();
}

#if defined(_WIN32)

bool IsDebuggerAttached()
{
    return ::IsDebuggerPresent() != 0;
}

#else

bool IsDebuggerAttached()
{
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line))
    {
        if (line.rfind("TracerPid:", 0) == 0)
            return std::strtol(line.c_str() + 10, nullptr, 10) != 0;
    }
    return false;
}

#endif

} // namespace x2::platform

#if !defined(_WIN32)
void OutputDebugStringW(const wchar_t* text)
{
    if (text != nullptr) {
        std::fputs(x2::platform::WideToUtf8(text).c_str(), stderr);
    }
}

void OutputDebugStringA(const char* text)
{
    if (text != nullptr) {
        std::fputs(text, stderr);
    }
}
#endif
