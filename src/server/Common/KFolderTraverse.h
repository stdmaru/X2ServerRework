#pragma once

#include "x2/platform/win32.h"

#include <filesystem>
#include <string>

std::wstring KGetCurFolder();
void KSetCurFolder(const WCHAR* pcName);
std::wstring KMakeAbsolutePath(const WCHAR* pcFile);
void KDeleteFolder(const WCHAR* strPath);

//---------------------------------------------------------------------------
// Calls tFunc(const std::filesystem::directory_entry&) for every file and folder under the current
// folder, depth first (a folder's contents come before the folder itself). The original walked
// FindFirstFile/FindNextFile and handed WIN32_FIND_DATA to the functor; it also skipped the first entry
// FindFirstFile returned (normally "."), which is why no entry is skipped here.
template <class TFunc>
void KApplyAllFiles(TFunc tFunc, const std::filesystem::path& folder = std::filesystem::path("."))
{
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(folder, ec))
    {
        if (entry.is_directory(ec))
            KApplyAllFiles(tFunc, entry.path());
        tFunc(entry);
    }
}
//---------------------------------------------------------------------------
