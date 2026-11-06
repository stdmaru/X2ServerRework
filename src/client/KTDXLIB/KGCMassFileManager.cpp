// KGCMassFileManager on x2::kom (decision T35). docs/porting/client-ktdxlib.md#mass-files

#include "KGCMassFileManager.h"

#include "KncUtil.h"

#include "x2/kom/codec.h"
#include "x2/kom/mount.h"
#include "x2/platform/file.h"
#include "x2/platform/unicode.h"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cwchar>
#include <cstring>
#include <mutex>
#include <set>
#include <system_error>

namespace fs = std::filesystem;

/*static*/ KGCMassFileBufferManager KGCMassFileManager::m_massFileBufMan;

namespace {

std::string PathUtf8(const fs::path& path)
{
    const std::u8string s = path.u8string();
    return std::string(s.begin(), s.end());
}

// For messages: '/' separators on every system.
std::string GenericUtf8(const fs::path& path)
{
    const std::u8string s = path.generic_u8string();
    return std::string(s.begin(), s.end());
}

std::wstring PathWide(const fs::path& path)
{
#if defined(_WIN32)
    return path.wstring();
#else
    return x2::platform::Utf8ToWide(PathUtf8(path));
#endif
}

fs::path WidePath(std::wstring_view name)
{
#if defined(_WIN32)
    return fs::path(std::wstring(name));
#else
    const std::string utf8 = x2::platform::WideToUtf8(name);
    return fs::path(std::u8string(utf8.begin(), utf8.end()));
#endif
}

std::string FileNameKey(const fs::path& path)
{
    return x2::kom::AsciiLower(PathUtf8(path.filename()));
}

bool IsHiddenFolder(const fs::path& path)
{
    const std::string name = PathUtf8(path.filename());
    return !name.empty() && name.front() == '.';
}

// A file of folder `dir` named `name`, compared case-insensitively (ASCII) on file systems that are not.
fs::path FindInFolder(const fs::path& dir, const fs::path& name)
{
    std::error_code ec;
    const fs::path exact = dir / name;
    if (fs::is_regular_file(exact, ec))
        return exact;
#if !defined(_WIN32)
    if (!name.has_parent_path())
    {
        const std::string key = FileNameKey(name);
        for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec))
            if (FileNameKey(it->path()) == key && it->is_regular_file(ec))
                return it->path();
    }
#endif
    return {};
}

// Stored blobs are read here before decoding; bounded so one huge entry does not pin memory in every thread.
std::vector<std::uint8_t>& Scratch()
{
    thread_local std::vector<std::uint8_t> scratch;
    return scratch;
}

void TrimScratch()
{
    constexpr std::size_t kKeep = 16u << 20;
    std::vector<std::uint8_t>& scratch = Scratch();
    if (scratch.capacity() > kKeep)
        std::vector<std::uint8_t>().swap(scratch);
}

} // namespace

KGCMassFileManager::KGCMassFileManager(LogSink log) : m_log(std::move(log))
{
    if (!m_log)
        m_log = [](const std::string& message) { std::fprintf(stderr, "mass file: %s\n", message.c_str()); };
    m_pMount = std::make_unique<x2::kom::Mount>([this](const std::string& warning) { Log("warning: " + warning); });
    InitServerCurrentTime();
}

KGCMassFileManager::~KGCMassFileManager() = default;

void KGCMassFileManager::Log(const std::string& message) const
{
    m_log(message);
}

std::string KGCMassFileManager::Key(std::wstring_view name)
{
    return x2::kom::AsciiLower(x2::platform::WideToUtf8(name));
}

// ---- Archives -------------------------------------------------------------------------------------------------------

bool KGCMassFileManager::AddMassFile(const fs::path& komFile, std::string& error)
{
    if (m_bLockMassFileMap)
    {
        error = PathUtf8(komFile) + ": the file map is locked";
        return false;
    }
    return m_pMount->AddArchive(komFile, error);
}

bool KGCMassFileManager::AddMassFiles(const fs::path& folder, const std::vector<std::wstring>& names,
                                      std::string& error)
{
    std::error_code ec;
    std::map<std::string, fs::path> present;  // .kom files of the folder by key
    for (fs::directory_iterator it(folder, ec), end; !ec && it != end; it.increment(ec))
    {
        const std::string key = FileNameKey(it->path());
        if (key.size() > 4 && key.ends_with(".kom") && it->is_regular_file(ec))
            present.emplace(key, it->path());
    }
    if (ec)
    {
        error = "cannot list " + PathUtf8(folder) + ": " + ec.message();
        return false;
    }

    std::set<std::string> listed;
    for (const std::wstring& name : names)
    {
        const std::string key = Key(name);
        listed.insert(key);
        const auto it = present.find(key);
        if (it == present.end())
        {
            error = x2::platform::WideToUtf8(name) + ": missing from " + PathUtf8(folder);
            return false;
        }
        if (!AddMassFile(it->second, error))
            return false;
    }
    for (const auto& [key, path] : present)
        if (!listed.contains(key))
            Log("warning: " + PathUtf8(path.filename()) + " is not in the client's archive list; ignored");
    return true;
}

/*static*/ std::vector<std::wstring> KGCMassFileManager::GetClientArchiveNames()
{
    std::vector<std::wstring> names;
    wchar_t name[32];
    for (int i = 1; i <= 150; ++i)
    {
        std::swprintf(name, std::size(name), L"data%03d.kom", i);
        names.emplace_back(name);
    }
    for (int i = 1; i <= 23; ++i)
    {
        std::swprintf(name, std::size(name), L"datai%03d.kom", i);
        names.emplace_back(name);
    }
    return names;
}

void KGCMassFileManager::LockMassFileMap()
{
    m_bLockMassFileMap = true;
}

int KGCMassFileManager::GetTotalFileCount() const
{
    return static_cast<int>(m_pMount->NameCount());
}

std::size_t KGCMassFileManager::GetMassFileCount() const
{
    return m_pMount->Archives().size();
}

// ---- Folders --------------------------------------------------------------------------------------------------------

void KGCMassFileManager::AddDataDirectory(const std::wstring& wstrDirectory, bool bInsertFront)
{
    const fs::path dir = WidePath(wstrDirectory);
    std::unique_lock lock(m_mutexDirectories);
    if (std::find(m_vecDataDirectory.begin(), m_vecDataDirectory.end(), dir) != m_vecDataDirectory.end())
        return;
    if (bInsertFront)
        m_vecDataDirectory.insert(m_vecDataDirectory.begin(), dir);
    else
        m_vecDataDirectory.push_back(dir);
}

void KGCMassFileManager::AddDataDirectory(const std::string& strDirectory, bool bInsertFront)
{
    AddDataDirectory(x2::platform::Utf8ToWide(strDirectory), bInsertFront);
}

bool KGCMassFileManager::SetSubDirectory()
{
    std::error_code ec;
    const fs::path current = fs::current_path(ec);
    if (ec)
    {
        Log("cannot get the current folder: " + ec.message());
        return false;
    }
    return SetSubDirectory(current);
}

bool KGCMassFileManager::SetSubDirectory(const fs::path& root)
{
    std::error_code ec;
    if (!fs::is_directory(root, ec))
    {
        Log(PathUtf8(root) + ": not a folder");
        return false;
    }

    fs::path looseFolder;  // root/data, any case
    for (fs::directory_iterator it(root, ec), end; !ec && it != end; it.increment(ec))
        if (FileNameKey(it->path()) == "data" && it->is_directory(ec))
            looseFolder = it->path();

    AddDataDirectory(PathWide(root));
    AddSubDirectories(root, looseFolder);
#if X2_DEV_TOOLS
    if (!looseFolder.empty())
        return SetLooseFileFolder(looseFolder);
#endif
    return true;
}

void KGCMassFileManager::AddSubDirectories(const fs::path& dir, const fs::path& skip)
{
    std::error_code ec;
    std::vector<fs::path> subdirs;
    for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec))
        if (it->is_directory(ec) && !IsHiddenFolder(it->path()) && it->path() != skip)
            subdirs.push_back(it->path());
    std::sort(subdirs.begin(), subdirs.end());
    for (const fs::path& sub : subdirs)
    {
        AddDataDirectory(PathWide(sub));
        AddSubDirectories(sub, skip);
    }
}

bool KGCMassFileManager::SetLooseFileFolder(const fs::path& folder)
{
    std::error_code ec;
    std::vector<fs::path> files;
    fs::recursive_directory_iterator it(folder, fs::directory_options::skip_permission_denied, ec);
    if (ec)
    {
        Log("cannot list " + PathUtf8(folder) + ": " + ec.message());
        return false;
    }
    for (const fs::recursive_directory_iterator end; it != end; it.increment(ec))
    {
        if (ec)
            break;
        if (it->is_directory(ec))
        {
            if (IsHiddenFolder(it->path()))
                it.disable_recursion_pending();
        }
        else if (it->is_regular_file(ec))
            files.push_back(it->path());
    }
    if (ec)
    {
        Log("cannot list " + PathUtf8(folder) + ": " + ec.message());
        return false;
    }
    std::sort(files.begin(), files.end());

    std::unordered_map<std::string, LooseFile> index;
    index.reserve(files.size());
    for (fs::path& path : files)
    {
        const auto [found, inserted] = index.try_emplace(FileNameKey(path), LooseFile{path});
        if (!inserted)
            Log("warning: " + PathUtf8(path.filename()) + " is in both " +
                GenericUtf8(found->second.path.lexically_relative(folder)) + " and " +
                GenericUtf8(path.lexically_relative(folder)) + "; using " +
                GenericUtf8(found->second.path.lexically_relative(folder)));
    }

    std::unique_lock lock(m_mutexDirectories);
    m_mapLooseFile.swap(index);
    return true;
}

std::size_t KGCMassFileManager::GetLooseFileCount() const
{
    std::shared_lock lock(m_mutexDirectories);
    return m_mapLooseFile.size();
}

fs::path KGCMassFileManager::FindLooseFile(const std::string& key) const
{
    std::shared_lock lock(m_mutexDirectories);
    const auto it = m_mapLooseFile.find(key);
    return it == m_mapLooseFile.end() ? fs::path() : it->second.path;
}

fs::path KGCMassFileManager::FindLocalFile(std::wstring_view wstrFileName) const
{
    if (wstrFileName.empty())
        return {};
    std::wstring name(wstrFileName);
    std::replace(name.begin(), name.end(), L'\\', L'/');
    const fs::path relative = WidePath(name).lexically_normal();
    {
        std::shared_lock lock(m_mutexDirectories);
        for (const fs::path& dir : m_vecDataDirectory)
        {
            fs::path found = FindInFolder(dir, relative);
            if (!found.empty())
                return found;
        }
    }
#if X2_DEV_TOOLS
    if (!relative.has_parent_path())
        return FindLooseFile(Key(name));
#endif
    return {};
}

// ---- Loading --------------------------------------------------------------------------------------------------------

std::string KGCMassFileManager::Resolve(const std::string& key) const
{
    std::shared_lock lock(m_mutexMapping);
    const auto it = m_mapActiveMapping.find(key);
    return it == m_mapActiveMapping.end() ? key : it->second;
}

bool KGCMassFileManager::LoadFromArchives(const std::string& key, MassFileInfo& out) const
{
    const x2::kom::Mount::Hit hit = m_pMount->Find(key);
    if (!hit)
        return false;
    out.strFileName = x2::platform::Utf8ToWide(hit.entry->name);
    out.size = static_cast<long>(hit.entry->size);
    if ((out.dwFlag & MFI_REALDATA) == 0)
        return true;

    KGCMassFileBufferPtr buffer = m_massFileBufMan.GetBuffer(static_cast<int>(hit.entry->size));
    std::string error;
    const bool ok = hit.archive->ReadInto(
        *hit.entry, std::span<std::uint8_t>(reinterpret_cast<std::uint8_t*>(buffer->GetBuffer()), hit.entry->size),
        Scratch(), error);
    TrimScratch();
    if (!ok)
    {
        Log(error);
        return true;  // found, but no data
    }
    out.pBuffer = std::move(buffer);
    out.pRealData = out.pBuffer->GetBuffer();
    return true;
}

bool KGCMassFileManager::LoadFromDisk(const fs::path& path, MassFileInfo& out) const
{
    std::string error;
    const std::unique_ptr<x2::platform::ReadOnlyFile> file = x2::platform::ReadOnlyFile::Open(path, error);
    if (!file)
    {
        Log(error);
        return false;
    }
    if (file->Size() > static_cast<std::uint64_t>(INT_MAX))
    {
        Log(PathUtf8(path) + ": too large");
        return false;
    }
    out.strFileName = PathWide(path.filename());
    out.size = static_cast<long>(file->Size());
    out.dwFlag |= MFI_LOCALFILE;
    if ((out.dwFlag & MFI_REALDATA) == 0)
        return true;

    KGCMassFileBufferPtr buffer = m_massFileBufMan.GetBuffer(static_cast<int>(out.size));
    if (out.size > 0 && !file->ReadAt(0, buffer->GetBuffer(), static_cast<std::size_t>(out.size), error))
    {
        Log(error);
        return true;  // found, but no data
    }
    out.pBuffer = std::move(buffer);
    out.pRealData = out.pBuffer->GetBuffer();
    return true;
}

bool KGCMassFileManager::LoadEntry(const std::wstring& wstrFileName, bool bRealData, MassFileInfo& out)
{
    out.dwFlag = bRealData ? MFI_REALDATA : 0;
    if (wstrFileName.empty())
        return false;
    const std::string key = Resolve(Key(wstrFileName));
#if X2_DEV_TOOLS
    const fs::path loose = FindLooseFile(key);
    if (!loose.empty())
        return LoadFromDisk(loose, out);
#endif
    return LoadFromArchives(key, out);
}

bool KGCMassFileManager::LoadLocal(const std::wstring& wstrFileName, bool bRealData, MassFileInfo& out)
{
    out.dwFlag = bRealData ? MFI_REALDATA : 0;
    const fs::path path = FindLocalFile(wstrFileName);
    return !path.empty() && LoadFromDisk(path, out);
}

bool KGCMassFileManager::IsValidFile(const std::wstring& wstrFileName)
{
    MassFileInfo info;
    return LoadEntry(wstrFileName, false, info);
}

KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_PTR
KGCMassFileManager::LoadDataFile(const std::wstring& wstrFileName, bool bRealData)
{
    CMassFile::MASSFILE_MEMBERFILEINFO_PTR info;
    if (!LoadEntry(wstrFileName, bRealData, info))
        Log(x2::platform::WideToUtf8(wstrFileName) + ": not found");
    return info;
}

KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_PTR
KGCMassFileManager::LoadDataFile(const std::string& strFileName, bool bRealData)
{
    return LoadDataFile(x2::platform::Utf8ToWide(strFileName), bRealData);
}

KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_PTR
KGCMassFileManager::LoadTwoDataFiles(const std::wstring& wstrFileName, const std::wstring& wstrFileName2,
                                     CMassFile::MASSFILE_MEMBERFILEINFO_PTR& info2, bool bRealData)
{
    info2 = CMassFile::MASSFILE_MEMBERFILEINFO_PTR();
    CMassFile::MASSFILE_MEMBERFILEINFO_PTR info;
    if (!LoadEntry(wstrFileName, bRealData, info))
    {
        Log(x2::platform::WideToUtf8(wstrFileName) + ": not found");
        return info;
    }
    if (!wstrFileName2.empty())
        LoadEntry(wstrFileName2, bRealData, info2);  // usually missing (.tet): not logged
    return info;
}

KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_PTR
KGCMassFileManager::LoadTwoDataFiles(const std::string& strFileName, const std::string& strFileName2,
                                     CMassFile::MASSFILE_MEMBERFILEINFO_PTR& info2, bool bRealData)
{
    return LoadTwoDataFiles(x2::platform::Utf8ToWide(strFileName), x2::platform::Utf8ToWide(strFileName2), info2,
                            bRealData);
}

bool KGCMassFileManager::IsValidFile_LocalFile(const std::wstring& wstrFileName)
{
    return !FindLocalFile(wstrFileName).empty();
}

KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_PTR
KGCMassFileManager::LoadDataFile_LocalFile(const std::wstring& wstrFileName, bool bRealData)
{
    CMassFile::MASSFILE_MEMBERFILEINFO_PTR info;
    LoadLocal(wstrFileName, bRealData, info);  // a missing user file is normal: not logged
    return info;
}

KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_PTR
KGCMassFileManager::LoadDataFile_LocalFile(const std::string& strFileName, bool bRealData)
{
    return LoadDataFile_LocalFile(x2::platform::Utf8ToWide(strFileName), bRealData);
}

WCHAR* KGCMassFileManager::GetPullPath(WCHAR* wcFileName)
{
    if (wcFileName == nullptr)
        return nullptr;
    const fs::path path = FindLocalFile(wcFileName);
    if (path.empty())
        return nullptr;
    const std::wstring full = PathWide(path);
    if (full.size() >= MAX_PATH)
    {
        Log(PathUtf8(path) + ": path longer than MAX_PATH");
        return nullptr;
    }
    std::copy(full.begin(), full.end(), wcFileName);
    wcFileName[full.size()] = L'\0';
    return wcFileName;
}

char* KGCMassFileManager::GetPullPath(char* cFileName)
{
    if (cFileName == nullptr)
        return nullptr;
    const fs::path path = FindLocalFile(x2::platform::Utf8ToWide(cFileName));
    if (path.empty())
        return nullptr;
    const std::string full = PathUtf8(path);
    if (full.size() >= MAX_PATH)
    {
        Log(full + ": path longer than MAX_PATH");
        return nullptr;
    }
    std::memcpy(cFileName, full.c_str(), full.size() + 1);
    return cFileName;
}

// ---- Date-range renames ---------------------------------------------------------------------------------------------

bool KGCMassFileManager::AddMappingList_LUA(int nIndex, const char* szDestMappingFileName,
                                            const char* szSrcMappingFileName)
{
    if (szDestMappingFileName == nullptr || szSrcMappingFileName == nullptr)
        return false;
    m_mapMassFileMappingList[nIndex].emplace(szDestMappingFileName, szSrcMappingFileName);
    return true;
}

bool KGCMassFileManager::AddMappingTime_LUA(int nIndex, const char* szStartDate, const char* szEndDate)
{
    if (szStartDate == nullptr || szEndDate == nullptr)
        return false;
    MappingTime time;
    if (!KncUtil::ConvertStringToCTime(x2::platform::Utf8ToWide(szStartDate), time.tStartTime) ||
        !KncUtil::ConvertStringToCTime(x2::platform::Utf8ToWide(szEndDate), time.tEndTime))
    {
        Log(std::string("mapping ") + std::to_string(nIndex) + ": bad date range \"" + szStartDate + "\" - \"" +
            szEndDate + "\"");
        return false;
    }
    m_mapMassFileMappingTime.emplace(nIndex, time);
    return true;
}

bool KGCMassFileManager::ExistsInArchivesOrLoose(const std::string& key) const
{
    if (m_pMount->Find(key))
        return true;
#if X2_DEV_TOOLS
    return !FindLooseFile(key).empty();
#else
    return false;
#endif
}

bool KGCMassFileManager::MassFileMapping()
{
    const CTime now(GetServerCurrentTime());
    std::unordered_map<std::string, std::string> active;
    for (const auto& [index, time] : m_mapMassFileMappingTime)
    {
        if (now < time.tStartTime || time.tEndTime < now)
            continue;
        const auto list = m_mapMassFileMappingList.find(index);
        if (list == m_mapMassFileMappingList.end())
            continue;
        for (const auto& [virtualName, realName] : list->second)
        {
            const std::string virtualKey = x2::kom::AsciiLower(virtualName);
            const std::string realKey = x2::kom::AsciiLower(realName);
            if (ExistsInArchivesOrLoose(virtualKey) && ExistsInArchivesOrLoose(realKey))
                active[virtualKey] = realKey;
        }
    }
    std::unique_lock lock(m_mutexMapping);
    m_mapActiveMapping.swap(active);
    return true;
}

bool KGCMassFileManager::ExternFileMapping(std::wstring& strExternFileName)
{
    const CTime now(GetServerCurrentTime());
    const std::string key = Key(strExternFileName);
    for (const auto& [index, time] : m_mapMassFileMappingTime)
    {
        if (now < time.tStartTime || time.tEndTime < now)
            continue;
        const auto list = m_mapMassFileMappingList.find(index);
        if (list == m_mapMassFileMappingList.end())
            continue;
        for (const auto& [virtualName, realName] : list->second)
        {
            if (x2::kom::AsciiLower(virtualName) != key)
                continue;
            strExternFileName = x2::platform::Utf8ToWide(realName);
            for (wchar_t& c : strExternFileName)
                if (c >= L'a' && c <= L'z')
                    c = static_cast<wchar_t>(c - L'a' + L'A');
            return true;
        }
    }
    return false;
}

void KGCMassFileManager::InitServerCurrentTime()
{
    // The client's clock until the server sends its time.
    m_ServerCurrentTime = CTime::GetCurrentTime();
    m_TickCountAtGetTime = ::GetTickCount();
}

__time64_t KGCMassFileManager::GetServerCurrentTime()
{
    const DWORD diffTickCount = ::GetTickCount() - m_TickCountAtGetTime;
    return m_ServerCurrentTime.GetTime() + (diffTickCount / 1000);
}

void KGCMassFileManager::SetServerCurrentTime(const std::wstring& wstrTime)
{
    KncUtil::ConvertStringToCTime(wstrTime, m_ServerCurrentTime);
    m_TickCountAtGetTime = ::GetTickCount();
}

// ---- Buffers --------------------------------------------------------------------------------------------------------

void KGCMassFileManager::ReleaseAllMemoryBuffers()
{
    m_massFileBufMan.ReleaseAllBuffers();
}
