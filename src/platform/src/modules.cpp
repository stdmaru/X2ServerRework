#include "x2/platform/modules.h"

#if defined(_WIN32)
#include "x2/platform/types.h" // <windows.h>
#include <psapi.h>
#else
#include "x2/platform/unicode.h"
#include <link.h>
#endif

namespace x2::platform {

std::wstring ModuleFileName(const std::wstring& path)
{
    const size_t slash = path.find_last_of(L"\\/");
    return slash == std::wstring::npos ? path : path.substr(slash + 1);
}

#if defined(_WIN32)

std::vector<LoadedModule> ListLoadedModules()
{
    std::vector<LoadedModule> modules;
    std::vector<HMODULE> handles(256);
    DWORD needed = 0;
    for (;;)
    {
        const DWORD bytes = static_cast<DWORD>(handles.size() * sizeof(HMODULE));
        if (!::EnumProcessModules(::GetCurrentProcess(), handles.data(), bytes, &needed))
            return modules;
        if (needed <= bytes)
            break;
        handles.resize(needed / sizeof(HMODULE));
    }
    handles.resize(needed / sizeof(HMODULE));

    for (HMODULE handle : handles)
    {
        wchar_t path[MAX_PATH] = {};
        if (::GetModuleFileNameW(handle, path, MAX_PATH) == 0)
            continue;
        modules.push_back({path, handle});
    }
    return modules;
}

bool UnloadModule(const LoadedModule& module)
{
    return module.handle != nullptr && ::FreeLibrary(static_cast<HMODULE>(module.handle)) != 0;
}

#else

std::vector<LoadedModule> ListLoadedModules()
{
    std::vector<LoadedModule> modules;
    dl_iterate_phdr(
        [](dl_phdr_info* info, size_t, void* data) -> int {
            auto& list = *static_cast<std::vector<LoadedModule>*>(data);
            std::string path = info->dlpi_name != nullptr ? info->dlpi_name : "";
            if (list.empty() && path.empty())
                path = "/proc/self/exe"; // the executable reports no name
            if (!path.empty())
                list.push_back({Utf8ToWide(path), reinterpret_cast<void*>(info->dlpi_addr)});
            return 0;
        },
        &modules);
    return modules;
}

bool UnloadModule(const LoadedModule&)
{
    return false;
}

#endif

} // namespace x2::platform
