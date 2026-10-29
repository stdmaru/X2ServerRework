#include "x2/platform/debug.h"
#include "x2/platform/unicode.h"

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

} // namespace x2::platform
