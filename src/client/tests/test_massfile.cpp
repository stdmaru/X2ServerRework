// KGCMassFileManager on x2::kom (docs/porting/client-ktdxlib.md#mass-files): archives the tests build, in the client's
// load order, first-wins, decoding by algorithm, full names, loose files preferred in dev builds, the folders of user
// files, date-range renames, threads and the buffer pool. Opt-in: X2_MASSFILE_CLIENT_DATA=<the client's data folder>
// mounts the real archives (and X2_MASSFILE_LOOSE=<a client_data folder> indexes it as loose files).

#include "KGCMassFileManager.h"

#include "x2/kom/mount.h"
#include "x2/kom/writer.h"
#include "x2/platform/unicode.h"

#include <doctest/doctest.h>

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

namespace {

using Bytes = std::vector<std::uint8_t>;

Bytes ToBytes(std::string_view s)
{
    return Bytes(s.begin(), s.end());
}

std::string Text(const KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_PTR& info)
{
    return info.pRealData ? std::string(info.pRealData, static_cast<std::size_t>(info.size)) : std::string();
}

// As the engine tests what LoadDataFile returns (`Info == NULL`, `Info != NULL`); outside doctest's expression
// decomposition, where NULL would no longer be a null pointer constant.
using InfoPtr = KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_PTR;
bool IsNull(const InfoPtr& info)
{
    return info == NULL;
}
bool IsSet(const InfoPtr& info)
{
    return info != NULL;
}

void WriteText(const fs::path& path, std::string_view text)
{
    fs::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(text.data(), static_cast<std::streamsize>(text.size()));
}

class TempDir
{
public:
    TempDir()
    {
        static std::atomic<unsigned> counter{0};
        const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        path_ = fs::temp_directory_path() / ("x2_massfile_test_" + std::to_string(stamp) + "_" +
                                             std::to_string(counter++));
        fs::create_directories(path_);
    }
    ~TempDir()
    {
        std::error_code ec;
        fs::remove_all(path_, ec);
    }
    const fs::path& Path() const { return path_; }
    fs::path operator/(const std::string& name) const { return path_ / name; }

private:
    fs::path path_;
};

struct File
{
    std::string name;
    std::string text;
    std::uint32_t algorithm = x2::kom::kZlib;
};

void BuildKom(const fs::path& path, const std::vector<File>& files)
{
    x2::kom::ArchiveBuilder builder;
    std::string error;
    for (const File& f : files)
        REQUIRE_MESSAGE(builder.Add(f.name, f.algorithm, ToBytes(f.text), error), error);
    REQUIRE_MESSAGE(builder.Write(path, error), error);
}

// A manager whose log is recorded.
struct Manager
{
    std::vector<std::string> log;
    KGCMassFileManager manager{[this](const std::string& m) { log.push_back(m); }};

    bool Logged(std::string_view part) const
    {
        for (const std::string& m : log)
            if (m.find(part) != std::string::npos)
                return true;
        return false;
    }
};

} // namespace

TEST_CASE("massfile: the client's archive list")
{
    const std::vector<std::wstring> names = KGCMassFileManager::GetClientArchiveNames();
    REQUIRE(names.size() == 173);
    CHECK(names.front() == L"data001.kom");
    CHECK(names[149] == L"data150.kom");
    CHECK(names[150] == L"datai001.kom");
    CHECK(names.back() == L"datai023.kom");
}

TEST_CASE("massfile: archives in list order, first wins, decoded by algorithm")
{
    TempDir dir;
    const std::string longName = "Trigger_Hamel_Frozen_Water_Hall_Stage3_Stone_Statue_Water_A.x";  // 61 characters
    BuildKom(dir / "data001.kom", {{"Shared.lua", "first", x2::kom::kXorCrc},
                                   {"UI_Game.dds", std::string(70000, 'u')},
                                   {"Empty.txt", ""}});
    BuildKom(dir / "DATA002.KOM", {{"shared.LUA", "second"},
                                   {longName, "long name"},
                                   {"Event.lua", "xorcrc3", x2::kom::kXorCrc3}});
    BuildKom(dir / "data999.kom", {{"Extra.txt", "not in the list"}});

    Manager m;
    std::string error;
    REQUIRE_MESSAGE(m.manager.AddMassFiles(dir.Path(), {L"data001.kom", L"data002.kom"}, error), error);
    m.manager.LockMassFileMap();

    CHECK(m.manager.GetMassFileCount() == 2);
    CHECK(m.manager.GetTotalFileCount() == 5);
    CHECK(m.Logged("shared.LUA is in both data001.kom and DATA002.KOM; using data001.kom"));
    CHECK(m.Logged("data999.kom is not in the client's archive list; ignored"));

    auto info = m.manager.LoadDataFile(std::wstring(L"SHARED.lua"));
    REQUIRE(IsSet(info));
    CHECK(Text(info) == "first");                 // algorithm 2, decrypted by the manager
    CHECK(info->strFileName == L"Shared.lua");    // the name as stored
    CHECK((info->dwFlag & MFI_LOCALFILE) == 0);

    CHECK(Text(m.manager.LoadDataFile(std::string("event.lua"))) == "xorcrc3");
    CHECK(Text(m.manager.LoadDataFile(std::string("ui_game.dds"))) == std::string(70000, 'u'));
    auto full = m.manager.LoadDataFile(x2::platform::Utf8ToWide(longName));
    CHECK(Text(full) == "long name");  // the original truncated names to 59 characters and never found it
    CHECK(full.strFileName.size() == 61);

    CHECK(m.manager.IsValidFile(L"empty.txt"));
    auto empty = m.manager.LoadDataFile(std::wstring(L"Empty.txt"));
    CHECK(IsNull(empty));  // no data, as the original: callers treat it as missing
    CHECK(empty.size == 0);

    auto sizeOnly = m.manager.LoadDataFile(std::wstring(L"ui_game.dds"), false);
    CHECK(sizeOnly.size == 70000);
    CHECK(sizeOnly.pRealData == nullptr);

    CHECK_FALSE(m.manager.IsValidFile(L"Extra.txt"));
    auto missing = m.manager.LoadDataFile(std::wstring(L"nothing.dds"));
    CHECK(IsNull(missing));
    CHECK(!missing);
    CHECK(m.Logged("nothing.dds: not found"));

    // Locked: no more archives.
    CHECK_FALSE(m.manager.AddMassFile(dir / "data999.kom", error));
    CHECK(error.find("locked") != std::string::npos);
}

TEST_CASE("massfile: a missing or rejected archive is named")
{
    TempDir dir;
    BuildKom(dir / "data001.kom", {{"a.txt", "a"}});
    {
        Manager m;
        std::string error;
        CHECK_FALSE(m.manager.AddMassFiles(dir.Path(), {L"data001.kom", L"data002.kom"}, error));
        CHECK(error.find("data002.kom: missing from") != std::string::npos);
    }
    {
        std::ofstream(dir / "data002.kom", std::ios::binary) << "not an archive";
        Manager m;
        std::string error;
        CHECK_FALSE(m.manager.AddMassFiles(dir.Path(), {L"data001.kom", L"data002.kom"}, error));
        CHECK(error.find("data002.kom") != std::string::npos);
    }
}

TEST_CASE("massfile: loose files under data/ (dev tools) and the folders of user files")
{
    TempDir root;
    BuildKom(root / "data001.kom", {{"Texture.dds", "from the archive"},
                                    {"Script.lua", "archive script"},
                                    {"Item.kim", "kim"}});
    WriteText(root / "data" / "ui" / "deep" / "TEXTURE.DDS", "loose texture");
    WriteText(root / "data" / "characters" / "Only_Loose.x", "only loose");
    WriteText(root / "data" / ".kom" / "blobs" / "Script.lua", "client_data's kept blob");  // skipped
    WriteText(root / "data" / "a" / "Twice.txt", "first");
    WriteText(root / "data" / "b" / "twice.txt", "second");
    WriteText(root / "GameOptions.lua", "options");
    WriteText(root / "music" / "Town.mp3", "music");
    WriteText(root / ".hidden" / "Secret.txt", "hidden");

    Manager m;
    REQUIRE(m.manager.SetSubDirectory(root.Path()));
    std::string error;
    REQUIRE_MESSAGE(m.manager.AddMassFiles(root.Path(), {L"data001.kom"}, error), error);
    m.manager.LockMassFileMap();

    auto texture = m.manager.LoadDataFile(std::wstring(L"texture.dds"));
    auto onlyLoose = m.manager.LoadDataFile(std::wstring(L"only_loose.X"));
#if X2_DEV_TOOLS
    CHECK(m.manager.GetLooseFileCount() == 3);  // twice.txt once
    CHECK(Text(texture) == "loose texture");
    CHECK((texture->dwFlag & MFI_LOCALFILE) != 0);
    CHECK(texture->strFileName == L"TEXTURE.DDS");
    CHECK(Text(onlyLoose) == "only loose");
    CHECK(Text(m.manager.LoadDataFile(std::wstring(L"twice.txt"))) == "first");
    CHECK(m.Logged("twice.txt is in both a/Twice.txt and b/twice.txt; using a/Twice.txt"));
#else
    CHECK(m.manager.GetLooseFileCount() == 0);
    CHECK(Text(texture) == "from the archive");
    CHECK(IsNull(onlyLoose));
#endif
    CHECK(Text(m.manager.LoadDataFile(std::wstring(L"script.lua"))) == "archive script");

    // User files: the game folder and its subfolders, in every build, never the archives.
    auto options = m.manager.LoadDataFile_LocalFile(std::wstring(L"GAMEOPTIONS.LUA"));
    CHECK(Text(options) == "options");
    CHECK((options->dwFlag & MFI_LOCALFILE) != 0);
    CHECK(m.manager.IsValidFile_LocalFile(L"town.mp3"));
    CHECK_FALSE(m.manager.IsValidFile_LocalFile(L"Item.kim"));
    CHECK_FALSE(m.manager.IsValidFile_LocalFile(L"Secret.txt"));
    CHECK(IsNull(m.manager.LoadDataFile_LocalFile(std::string("missing.lua"))));

    WCHAR path[MAX_PATH] = L"Town.mp3";
    REQUIRE(m.manager.GetPullPath(path) != nullptr);
    CHECK(fs::equivalent(fs::path(path), root / "music" / "Town.mp3"));
    char narrow[MAX_PATH] = "music/Town.mp3";
    REQUIRE(m.manager.GetPullPath(narrow) != nullptr);
    CHECK(fs::equivalent(fs::path(narrow), root / "music" / "Town.mp3"));
    WCHAR nothing[MAX_PATH] = L"nothing.txt";
    CHECK(m.manager.GetPullPath(nothing) == nullptr);
#if X2_DEV_TOOLS
    WCHAR loose[MAX_PATH] = L"Only_Loose.x";  // the loose files come after the data directories
    CHECK(m.manager.GetPullPath(loose) != nullptr);
#endif

    // New loose files show up after indexing again.
    WriteText(root / "data" / "New.txt", "new");
    REQUIRE(m.manager.SetLooseFileFolder(root / "data"));
#if X2_DEV_TOOLS
    CHECK(Text(m.manager.LoadDataFile(std::wstring(L"new.txt"))) == "new");
#endif
}

TEST_CASE("massfile: two files at once")
{
    TempDir dir;
    BuildKom(dir / "data001.kom", {{"Tex.dds", "texture"}, {"Tex.tet", "uv"}, {"Other.dds", "other"}});
    Manager m;
    std::string error;
    REQUIRE(m.manager.AddMassFiles(dir.Path(), {L"data001.kom"}, error));

    KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_PTR info2;
    auto info = m.manager.LoadTwoDataFiles(std::wstring(L"tex.dds"), std::wstring(L"tex.tet"), info2);
    CHECK(Text(info) == "texture");
    CHECK(Text(info2) == "uv");

    info = m.manager.LoadTwoDataFiles(std::string("other.dds"), std::string("other.tet"), info2);
    CHECK(Text(info) == "other");
    CHECK(IsNull(info2));
    CHECK_FALSE(m.Logged("other.tet"));  // a missing .tet is normal
}

TEST_CASE("massfile: date-range renames (MassFileMappingList.lua)")
{
    TempDir dir;
    BuildKom(dir / "data001.kom", {{"DLG_Login0.tga", "normal"}, {"DLG_Login0_1.tga", "event"}, {"A.dds", "a"}});
    Manager m;
    std::string error;
    REQUIRE(m.manager.AddMassFiles(dir.Path(), {L"data001.kom"}, error));
    m.manager.LockMassFileMap();

    CHECK(m.manager.AddMappingTime_LUA(2, "2014-04-02 00:00:00", "2014-04-08 23:59:00"));
    CHECK(m.manager.AddMappingList_LUA(2, "DLG_Login0.tga", "DLG_Login0_1.tga"));
    CHECK(m.manager.AddMappingList_LUA(2, "A.dds", "Missing.dds"));  // the target must exist
    CHECK_FALSE(m.manager.AddMappingTime_LUA(3, "someday", "2014-04-08 23:59:00"));

    m.manager.SetServerCurrentTime(L"2014-04-05 12:00:00");
    REQUIRE(m.manager.MassFileMapping());
    CHECK(Text(m.manager.LoadDataFile(std::wstring(L"dlg_login0.tga"))) == "event");
    CHECK(Text(m.manager.LoadDataFile(std::wstring(L"A.dds"))) == "a");
    std::wstring name = L"DLG_LOGIN0.TGA";
    CHECK(m.manager.ExternFileMapping(name));
    CHECK(name == L"DLG_LOGIN0_1.TGA");

    m.manager.SetServerCurrentTime(L"2014-04-09 00:00:00");
    REQUIRE(m.manager.MassFileMapping());
    CHECK(Text(m.manager.LoadDataFile(std::wstring(L"DLG_Login0.tga"))) == "normal");
    name = L"DLG_LOGIN0.TGA";
    CHECK_FALSE(m.manager.ExternFileMapping(name));
}

TEST_CASE("massfile: threads load at once")
{
    TempDir dir;
    std::vector<File> files;
    for (int i = 0; i < 64; ++i)
        files.push_back({"File" + std::to_string(i) + ".bin", std::string(1000 + i * 997, static_cast<char>('a' + i % 26)),
                         i % 3 == 0 ? x2::kom::kXorCrc : (i % 3 == 1 ? x2::kom::kXorCrc3 : x2::kom::kZlib)});
    BuildKom(dir / "data001.kom", files);
    Manager m;
    std::string error;
    REQUIRE(m.manager.AddMassFiles(dir.Path(), {L"data001.kom"}, error));
    m.manager.LockMassFileMap();

    std::atomic<int> failures{0};
    std::vector<std::thread> threads;
    for (int t = 0; t < 8; ++t)
        threads.emplace_back([&, t] {
            for (int round = 0; round < 20; ++round)
                for (int i = 0; i < 64; ++i)
                {
                    const File& f = files[static_cast<std::size_t>((i + t * 7) % 64)];
                    if (Text(m.manager.LoadDataFile(x2::platform::Utf8ToWide(f.name))) != f.text)
                        ++failures;
                }
        });
    for (std::thread& t : threads)
        t.join();
    CHECK(failures == 0);
}

TEST_CASE("massfile: buffers are pooled and outlive the info")
{
    KGCMassFileBufferManager pool;
    KGCMassFileBufferPtr kept;
    {
        KGCMassFileBufferPtr a = pool.GetBuffer(1000);
        REQUIRE(a->GetMemorySize() == 1024);
        std::memcpy(a->GetBuffer(), "kept", 5);
        kept = a;
    }
    CHECK(pool.GetPooledCount() == 0);  // still held
    CHECK(std::strcmp(kept->GetBuffer(), "kept") == 0);
    KGCMassFileBuffer* raw = kept.get();
    kept.reset();
    CHECK(pool.GetPooledCount() == 1);
    CHECK(pool.GetBuffer(600).get() == raw);  // same size class: reused
    CHECK(pool.GetPooledCount() == 1);        // and back again
    CHECK(pool.GetBuffer(5u << 20)->GetMemorySize() == 5 << 20);  // not pooled
    CHECK(pool.GetBuffer(0)->GetBuffer() == nullptr);
    pool.ReleaseAllBuffers();
    CHECK(pool.GetPooledCount() == 0);

    KGCMassFileBufferPtr survivor;
    {
        KGCMassFileBufferManager shortLived;
        survivor = shortLived.GetBuffer(10);
    }
    survivor.reset();  // the manager is gone: freed, not returned
}

TEST_CASE("massfile: the client's archives (X2_MASSFILE_CLIENT_DATA)")
{
    const char* folder = std::getenv("X2_MASSFILE_CLIENT_DATA");
    if (folder == nullptr || *folder == '\0')
    {
        MESSAGE("X2_MASSFILE_CLIENT_DATA not set: skipped");
        return;
    }
    Manager m;
    std::string error;
    const auto start = std::chrono::steady_clock::now();
    REQUIRE_MESSAGE(m.manager.AddMassFiles(folder, KGCMassFileManager::GetClientArchiveNames(), error), error);
    m.manager.LockMassFileMap();
    const auto mounted = std::chrono::steady_clock::now();
    CHECK(m.manager.GetMassFileCount() == 173);
    MESSAGE("names: " << m.manager.GetTotalFileCount() << ", mounted in "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(mounted - start).count() << " ms");
    for (const std::string& line : m.log)
        MESSAGE(line);

    // Every script and table the client reads first, and the name the original could not find.
    for (const wchar_t* name : {L"Enum.lua", L"Item.kim", L"MassFileMappingList.lua", L"GameOption.txt",
                                L"Trigger_Hamel_Frozen_Water_Hall_Stage3_Stone_Statue_Water_A.x"})
    {
        CAPTURE(x2::platform::WideToUtf8(name));
        auto info = m.manager.LoadDataFile(std::wstring(name));
        CHECK(IsSet(info));
        CHECK(info.size > 0);
    }

    // Every entry, decoded through the manager by 8 threads (names from x2::kom's own listing).
    x2::kom::Mount listing([](const std::string&) {});
    for (const std::wstring& name : KGCMassFileManager::GetClientArchiveNames())
        REQUIRE_MESSAGE(listing.AddArchive(fs::path(folder) / x2::platform::WideToUtf8(name), error), error);
    std::vector<const x2::kom::Entry*> entries;
    for (const auto& archive : listing.Archives())
        for (const x2::kom::Entry& entry : archive->Entries())
            entries.push_back(&entry);
    std::atomic<std::size_t> next{0}, failures{0};
    std::atomic<std::uint64_t> bytes{0};
    std::vector<std::thread> threads;
    for (int t = 0; t < 8; ++t)
        threads.emplace_back([&] {
            for (std::size_t i = next++; i < entries.size(); i = next++)
            {
                const x2::kom::Entry& entry = *entries[i];
                auto info = m.manager.LoadDataFile(x2::platform::Utf8ToWide(entry.name));
                if (static_cast<std::uint32_t>(info.size) != entry.size || (entry.size > 0 && info.pRealData == nullptr))
                    ++failures;
                bytes += static_cast<std::uint64_t>(info.size);
            }
        });
    for (std::thread& t : threads)
        t.join();
    CHECK(failures == 0);
    MESSAGE(entries.size() << " entries, " << bytes / (1024 * 1024) << " MB decoded in "
                           << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() -
                                                                               mounted)
                                  .count()
                           << " s");

    if (const char* loose = std::getenv("X2_MASSFILE_LOOSE"); loose != nullptr && *loose != '\0')
    {
        REQUIRE(m.manager.SetLooseFileFolder(loose));
        MESSAGE("loose files: " << m.manager.GetLooseFileCount());
#if X2_DEV_TOOLS
        auto script = m.manager.LoadDataFile(std::wstring(L"Enum.lua"));
        CHECK((script->dwFlag & MFI_LOCALFILE) != 0);
        CHECK(std::string_view(script.pRealData, 3) == "\xEF\xBB\xBF");  // client_data's readable Lua
#endif
    }
}
