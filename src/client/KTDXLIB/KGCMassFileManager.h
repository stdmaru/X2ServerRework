// KGCMassFileManager: the client's files by name, from the .kom archives and from folders on disk
// (KOG GC team; ported onto x2::kom, decision T35). docs/porting/client-ktdxlib.md#mass-files
//
//   KGCMassFileManager manager;
//   manager.SetSubDirectory( gameFolder );             // folders for user files, music, movies (every build); with
//                                                      // X2_DEV_TOOLS also indexes gameFolder/data (loose assets)
//   manager.AddMassFiles( gameFolder, komNames, error ); // CX2Main's fixed list, in load order
//   manager.LockMassFileMap();
//
//   KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_POINTER info = manager.LoadDataFile( L"ui_game.dds" );
//   if ( info != NULL ) use( info->pRealData, info->size );  // decoded data; info keeps its buffer alive
//
// Names are entry names (no folders), compared case-insensitively (ASCII), never truncated. Lookups:
// - LoadDataFile / IsValidFile / LoadTwoDataFiles: with X2_DEV_TOOLS a loose file of that name under the loose-file
//   folder first (flagged MFI_LOCALFILE), then the archives; release builds read the archives only.
// - *_LocalFile and GetPullPath: the data directories (SetSubDirectory, AddDataDirectory) in order, then with
//   X2_DEV_TOOLS the loose-file folder; never the archives. Every build.
// Thread-safe: any number of threads load at once (positional reads); mounting happens before LockMassFileMap.
#pragma once

#include "KGCMassFileBufferManager.h"

#include "x2/platform/time.h"
#include "x2/platform/types.h"

#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace x2::kom {
class Mount;
}

#define MFI_REALDATA            (1)     // load the data, not only the size
#define MFI_LOCALFILE           (4)     // the data came from a file on disk, not from an archive

class KGCMassFileManager
{
public:
    // Kept as the scope of the info types the engine names (KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_*);
    // the archives themselves are x2::kom's.
    struct CMassFile
    {
        struct MASSFILE_MEMBERFILEINFO
        {
            KGCMassFileBufferPtr pBuffer;   // owns pRealData
            std::wstring strFileName;       // the name as stored (archive entry or file on disk)
            long size = 0;                  // decoded size
            char* pRealData = nullptr;      // with MFI_REALDATA; nullptr if not found or not loaded
            DWORD dwFlag = MFI_REALDATA;
        };

        // What LoadDataFile returns: tests like the pointer it used to be (`info == NULL`, `!info`, `info->size`).
        struct MASSFILE_MEMBERFILEINFO_POINTER : public MASSFILE_MEMBERFILEINFO
        {
            MASSFILE_MEMBERFILEINFO* operator->() { return this; }
            const MASSFILE_MEMBERFILEINFO* operator->() const { return this; }
            operator const MASSFILE_MEMBERFILEINFO*() const { return this; }
            bool operator!() const { return pRealData == nullptr; }
            bool operator==(const MASSFILE_MEMBERFILEINFO* ptr) const
            {
                return ptr ? pRealData == ptr->pRealData : pRealData == nullptr;
            }
            bool operator!=(const MASSFILE_MEMBERFILEINFO* ptr) const { return !(*this == ptr); }
        };

        typedef MASSFILE_MEMBERFILEINFO_POINTER MASSFILE_MEMBERFILEINFO_PTR;
    };

    // Errors and warnings (missing files, duplicate names, unreadable entries). Default: stderr.
    using LogSink = std::function<void(const std::string&)>;

    explicit KGCMassFileManager(LogSink log = {});
    ~KGCMassFileManager();
    KGCMassFileManager(const KGCMassFileManager&) = delete;
    KGCMassFileManager& operator=(const KGCMassFileManager&) = delete;

    // ---- Archives (before LockMassFileMap) -------------------------------------------------------------------------

    // Mounts one archive after those already mounted. A name already mounted keeps its first entry (first wins, with
    // a warning). False and `error` naming the archive if it cannot be opened or the map is locked.
    bool AddMassFile(const std::filesystem::path& komFile, std::string& error);

    // Mounts folder/<name> for each name, in that order (the client's load order), stopping at the first archive that
    // cannot be opened. Other .kom files in the folder are reported as ignored.
    bool AddMassFiles(const std::filesystem::path& folder, const std::vector<std::wstring>& names, std::string& error);

    // The client's archives in load order: data001.kom .. data150.kom, then datai001.kom .. datai023.kom (CX2Main's
    // fixed list).
    static std::vector<std::wstring> GetClientArchiveNames();

    // After this the archives cannot change, which lets loading threads read the map without a lock.
    void LockMassFileMap();

    int GetTotalFileCount() const;     // names in the archives
    std::size_t GetMassFileCount() const;

    // ---- Folders --------------------------------------------------------------------------------------------------

    // Data directories (searched by *_LocalFile and GetPullPath, in order). Paths are folders; a trailing separator
    // is optional.
    void AddDataDirectory(const std::wstring& wstrDirectory, bool bInsertFront = false);
    void AddDataDirectory(const std::string& strDirectory, bool bInsertFront = false);

    // Registers root and its subfolders, recursively, as data directories (skipping folders whose name starts with
    // '.' and root/data). With X2_DEV_TOOLS, also SetLooseFileFolder(root/data) when that folder exists. The overload
    // without arguments uses the current folder, as the original.
    bool SetSubDirectory();
    bool SetSubDirectory(const std::filesystem::path& root);

    // Indexes every file under `folder`, recursively, by name (skipping folders whose name starts with '.', such as a
    // client_data's .kom). A name in two folders: the first in sorted path order wins, with a warning. Read only in
    // builds with X2_DEV_TOOLS. Can be called again to pick up new files.
    bool SetLooseFileFolder(const std::filesystem::path& folder);
    std::size_t GetLooseFileCount() const;

    // ---- Loading --------------------------------------------------------------------------------------------------

    bool IsValidFile(const std::wstring& wstrFileName);
    CMassFile::MASSFILE_MEMBERFILEINFO_PTR LoadDataFile(const std::string& strFileName, bool bRealData = true);
    CMassFile::MASSFILE_MEMBERFILEINFO_PTR LoadDataFile(const std::wstring& wstrFileName, bool bRealData = true);

    // Two files at once (a texture and its .tet). info2 stays empty if the second one is missing or empty.
    CMassFile::MASSFILE_MEMBERFILEINFO_PTR LoadTwoDataFiles(const std::string& strFileName,
                                                            const std::string& strFileName2,
                                                            CMassFile::MASSFILE_MEMBERFILEINFO_PTR& info2,
                                                            bool bRealData = true);
    CMassFile::MASSFILE_MEMBERFILEINFO_PTR LoadTwoDataFiles(const std::wstring& wstrFileName,
                                                            const std::wstring& wstrFileName2,
                                                            CMassFile::MASSFILE_MEMBERFILEINFO_PTR& info2,
                                                            bool bRealData = true);

    // Files on disk only (user settings such as GameOptions.lua), in every build.
    bool IsValidFile_LocalFile(const std::wstring& wstrFileName);
    CMassFile::MASSFILE_MEMBERFILEINFO_PTR LoadDataFile_LocalFile(const std::string& strFileName,
                                                                  bool bRealData = true);
    CMassFile::MASSFILE_MEMBERFILEINFO_PTR LoadDataFile_LocalFile(const std::wstring& wstrFileName,
                                                                  bool bRealData = true);

    // Replaces the file name in the buffer (MAX_PATH characters) with the full path of the file on disk; nullptr if
    // there is none. The char version uses UTF-8.
    WCHAR* GetPullPath(WCHAR* wcFileName);
    char* GetPullPath(char* cFileName);
    std::filesystem::path FindLocalFile(std::wstring_view wstrFileName) const;

    // ---- Date-range renames (MassFileMappingList.lua) -------------------------------------------------------------

    // Mapping nIndex: while the server time is within its range, loading the first name loads the second.
    bool AddMappingList_LUA(int nIndex, const char* strDestMappingFileName, const char* strSrcMappingFileName);
    bool AddMappingTime_LUA(int nIndex, const char* szStartDate, const char* szEndDate);

    // Applies the mappings in range at the current server time (the names must both exist), replacing the previous
    // ones. Called when the server time arrives. Safe while loading threads run.
    bool MassFileMapping();
    // The name strExternFileName is mapped to now (upper-cased, as the original), if any.
    bool ExternFileMapping(std::wstring& strExternFileName);

    void InitServerCurrentTime();
    __time64_t GetServerCurrentTime();
    void SetServerCurrentTime(const std::wstring& wstrTime);

    // ---- Buffers --------------------------------------------------------------------------------------------------

    void ReleaseAllMemoryBuffers();
    static KGCMassFileBufferManager& GetMassFileBufMan() { return m_massFileBufMan; }

private:
    struct LooseFile
    {
        std::filesystem::path path;
    };

    struct MappingTime
    {
        CTime tStartTime;
        CTime tEndTime;
    };

    using MassFileInfo = CMassFile::MASSFILE_MEMBERFILEINFO;

    // Lower-cased (ASCII) UTF-8 key of a name.
    static std::string Key(std::wstring_view name);
    std::string Resolve(const std::string& key) const;

    bool LoadFromArchives(const std::string& key, MassFileInfo& out) const;
    bool LoadFromDisk(const std::filesystem::path& path, MassFileInfo& out) const;
    bool LoadEntry(const std::wstring& wstrFileName, bool bRealData, MassFileInfo& out);
    bool LoadLocal(const std::wstring& wstrFileName, bool bRealData, MassFileInfo& out);
    std::filesystem::path FindLooseFile(const std::string& key) const;
    bool ExistsInArchivesOrLoose(const std::string& key) const;
    void AddSubDirectories(const std::filesystem::path& dir, const std::filesystem::path& skip);

    void Log(const std::string& message) const;

    LogSink m_log;

    std::unique_ptr<x2::kom::Mount> m_pMount;
    bool m_bLockMassFileMap = false;

    mutable std::shared_mutex m_mutexDirectories;      // data directories and loose index
    std::vector<std::filesystem::path> m_vecDataDirectory;
    std::unordered_map<std::string, LooseFile> m_mapLooseFile;

    std::map<int, std::map<std::string, std::string>> m_mapMassFileMappingList;
    std::map<int, MappingTime> m_mapMassFileMappingTime;
    mutable std::shared_mutex m_mutexMapping;
    std::unordered_map<std::string, std::string> m_mapActiveMapping;   // key -> key

    CTime m_ServerCurrentTime;
    DWORD m_TickCountAtGetTime = 0;

    static KGCMassFileBufferManager m_massFileBufMan;
};
