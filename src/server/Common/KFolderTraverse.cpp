#include "KFolderTraverse.h"

#include "x2/platform/unicode.h"

namespace fs = std::filesystem;

//---------------------------------------------------------------------------
void KSetCurFolder(const WCHAR* pcName)
{
    std::error_code ec;
    fs::current_path(fs::path(pcName), ec);
}
//---------------------------------------------------------------------------
std::wstring KGetCurFolder()
{
    std::error_code ec;
    return fs::current_path(ec).wstring();
}
//---------------------------------------------------------------------------
std::wstring KMakeAbsolutePath(const WCHAR* pcFile)
{
    std::error_code ec;
    return (fs::current_path(ec) / pcFile).wstring();
}
//---------------------------------------------------------------------------
// Deletes the folder and everything in it (recursively).
void KDeleteFolder(const WCHAR* strPath)
{
    std::error_code ec;
    fs::remove_all(fs::path(strPath), ec);
}
//---------------------------------------------------------------------------
