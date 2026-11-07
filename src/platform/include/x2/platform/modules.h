#pragma once

// The modules (DLLs / shared objects) loaded in this process, for the client's own anti-cheat (CX2DLLManager): the
// server names a module and the client unloads it.

#include <string>
#include <vector>

namespace x2::platform {

struct LoadedModule
{
    std::wstring path;      // full path, as the system reports it
    void* handle = nullptr; // HMODULE on Windows; the load address elsewhere
};

// Every module loaded in this process; the executable comes first.
std::vector<LoadedModule> ListLoadedModules();

// The file name of a module path (`C:\x\Foo.dll` -> `Foo.dll`, `/usr/lib/libfoo.so` -> `libfoo.so`).
std::wstring ModuleFileName(const std::wstring& path);

// Unloads a module from ListLoadedModules() (FreeLibrary). Only Windows can drop a module the process did not load
// itself, so elsewhere it does nothing and returns false.
bool UnloadModule(const LoadedModule& module);

} // namespace x2::platform
