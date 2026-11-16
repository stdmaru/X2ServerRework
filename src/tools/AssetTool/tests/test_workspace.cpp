// The manager's view of a client_data: file states, archive edits, sync, and packing what a sync added.
#include "helpers.h"

#include "x2/assets/client_data.h"
#include "x2/assets/workspace.h"
#include "x2/kom/mount.h"

using namespace x2::assets;
using namespace assets_test;
namespace fs = std::filesystem;

namespace {

struct Fixture
{
    TempDir dir;
    fs::path source = dir / "data";
    fs::path clientData = dir / "client_data";
    fs::path out = dir / "out";

    Fixture()
    {
        fs::create_directories(source);
        std::string error;
        x2::kom::ArchiveBuilder a;  // ui
        a.SetLevel(x2::kom::kKogLevel);
        REQUIRE(a.Add("DLG_Main.lua", x2::kom::kZlib, Pattern(500, 1), error));
        REQUIRE(a.Add("DLG_Other.lua", x2::kom::kZlib, Pattern(600, 2), error));
        REQUIRE(a.Add("Wall.dds", x2::kom::kXorCrc, Pattern(700, 3), error));
        REQUIRE(a.Add("Floor.dds", x2::kom::kXorCrc, Pattern(800, 4), error));
        REQUIRE_MESSAGE(a.Write(source / "a.kom", error), error);
        x2::kom::ArchiveBuilder b;  // sound
        b.SetLevel(x2::kom::kKogLevel);
        REQUIRE(b.Add("Hit.ogg", x2::kom::kZlib, Pattern(900, 5), error));
        REQUIRE(b.Add("Miss.ogg", x2::kom::kZlib, Pattern(1000, 6), error));
        REQUIRE_MESSAGE(b.Write(source / "b.kom", error), error);

        ImportOptions options;
        options.source = source;
        options.destination = clientData;
        REQUIRE(options.layout.Parse("ui/scripts   *.lua\n"
                                     "ui/textures  *.dds\n"
                                     "sound        *.ogg\n",
                                     error));
        options.jobs = 2;
        options.readableLua = false;
        ImportStats stats;
        REQUIRE_MESSAGE(Import(options, stats, error), error);
    }

    ClientData Open()
    {
        ClientData data;
        std::string error;
        REQUIRE_MESSAGE(data.Open(clientData, 2, error), error);
        return data;
    }

    PackStats DoPack()
    {
        PackOptions options;
        options.clientData = clientData;
        options.destination = out;
        options.jobs = 2;
        PackStats stats;
        std::string error;
        REQUIRE_MESSAGE(Pack(options, stats, error), error);
        return stats;
    }
};

FileState StateOf(const ClientData& data, const char* name)
{
    const ClientDataFile* f = data.Find(name);
    REQUIRE_MESSAGE(f, name);
    return f->state;
}

std::vector<std::string> EntryNames(const fs::path& archivePath)
{
    std::string error;
    auto archive = x2::kom::Archive::Open(archivePath, error);
    REQUIRE_MESSAGE(archive, error);
    std::vector<std::string> names;
    for (const auto& e : archive->Entries())
        names.push_back(e.name);
    return names;
}

} // namespace

TEST_CASE("assets: the manager finds files by name and sees each one's state")
{
    Fixture f;
    WriteFile(f.clientData / "ui" / "textures" / "Wall.dds", Pattern(10, 7));  // modified
    fs::remove(f.clientData / "sound" / "Miss.ogg");                            // deleted
    WriteFile(f.clientData / "ui" / "scripts" / "New.lua", ToBytes("x = 1"));  // untracked
    fs::create_directories(f.clientData / "moved");
    fs::rename(f.clientData / "ui" / "scripts" / "DLG_Other.lua", f.clientData / "moved" / "DLG_Other.lua");

    ClientData data = f.Open();
    CHECK(StateOf(data, "DLG_Main.lua") == FileState::Unchanged);
    CHECK(StateOf(data, "dlg_other.LUA") == FileState::Unchanged);  // moved: found by name
    CHECK(StateOf(data, "Wall.dds") == FileState::Modified);
    CHECK(StateOf(data, "Miss.ogg") == FileState::Deleted);
    CHECK(StateOf(data, "New.lua") == FileState::Untracked);
    CHECK(data.Find("Nothing.dds") == nullptr);

    CHECK(data.Locate("DLG_Other.lua") == f.clientData / "moved" / "DLG_Other.lua");
    CHECK(data.Locate("Miss.ogg").empty());
    CHECK(data.Find("DLG_Other.lua")->folder == "moved");

    // A new file goes next to a sibling, wherever that is now; else to the root.
    const std::vector<std::string> siblings{"Missing.lua", "DLG_Main.lua"};
    CHECK(data.FolderForNew("UIAnchors.lua", siblings) == "ui/scripts");
    CHECK(data.FolderForNew("New.lua") == "ui/scripts");  // already there
    CHECK(data.FolderForNew("Other.txt").empty());

    // The default archive: the most common under the folder, walking up.
    CHECK(data.DefaultArchive("ui/scripts") == "a.kom");
    CHECK(data.DefaultArchive("sound") == "b.kom");
    CHECK(data.DefaultArchive("sound/new/deeper") == "b.kom");
    CHECK(data.DefaultArchive("") == "a.kom");  // 4 of 5 remaining tracked files
    CHECK(data.ArchiveOf(*data.Find("New.lua")) == "a.kom");

    // The hash cache gives the same states on the next run.
    CHECK(fs::exists(HashCachePath(f.clientData)));
    ClientData again = f.Open();
    CHECK(StateOf(again, "Wall.dds") == FileState::Modified);
    CHECK(StateOf(again, "DLG_Main.lua") == FileState::Unchanged);
}

TEST_CASE("assets: archive edits wait for a sync, which records new files and drops deleted ones")
{
    Fixture f;
    const Bytes newSound = Pattern(1234, 8);
    WriteFile(f.clientData / "sound" / "Jump.ogg", newSound);
    WriteFile(f.clientData / "ui" / "textures" / "Sky.dds", Pattern(321, 9));
    fs::remove(f.clientData / "sound" / "Hit.ogg");
    WriteFile(f.clientData / "ui" / "textures" / "Floor.dds", Pattern(11, 10));  // modified, then moved to b.kom

    ClientData data = f.Open();
    std::string error;
    CHECK_FALSE(data.SetArchive("Hit.ogg", "a.kom", error));      // deleted
    CHECK_FALSE(data.SetArchive("Jump.ogg", "c.kom", error));     // no such archive
    CHECK(data.SetArchive("Jump.ogg", "b.kom", error));           // its default: no edit
    CHECK(data.EditCount() == 0);
    std::size_t count = 0;
    REQUIRE_MESSAGE(data.SetFolderArchive("ui/textures", "B.KOM", count, error), error);
    CHECK(count == 3);  // Wall, Floor, Sky
    CHECK(data.EditCount() == 3);
    CHECK(data.SetArchive("Wall.dds", "a.kom", error));  // back to its own
    CHECK(data.EditCount() == 2);
    CHECK(data.HasArchiveEdit(*data.Find("Floor.dds")));
    CHECK_FALSE(data.SetFolderArchive("../outside", "a.kom", count, error));

    SyncStats stats;
    REQUIRE_MESSAGE(data.Sync(2, stats, error), error);
    CHECK(stats.added == 2);  // Jump.ogg, Sky.dds
    CHECK(stats.removed == 1);
    CHECK(stats.moved == 1);  // Floor.dds
    CHECK(data.EditCount() == 0);
    CHECK(StateOf(data, "Jump.ogg") == FileState::Added);
    CHECK(StateOf(data, "Sky.dds") == FileState::Added);
    CHECK(StateOf(data, "Floor.dds") == FileState::Modified);  // still differs from its original
    CHECK(data.Find("Hit.ogg") == nullptr);

    const Manifest& m = data.GetManifest();
    REQUIRE(m.archives.size() == 2);
    CHECK(m.archives[0].count == 3);
    CHECK(m.archives[1].count == 4);
    const auto record = [&](const char* name) {
        const auto it = std::find_if(m.files.begin(), m.files.end(), [&](const FileRecord& r) { return r.name == name; });
        REQUIRE(it != m.files.end());
        return *it;
    };
    CHECK(record("Sky.dds").archive == "b.kom");
    CHECK(record("Sky.dds").encoding == Encoding::New);
    CHECK(record("Sky.dds").algorithm == x2::kom::kZlib);  // b.kom's own
    CHECK(record("Floor.dds").archive == "b.kom");
    CHECK(record("Floor.dds").index == 2);
    CHECK(record("Jump.ogg").index == 3);
    CHECK(record("Sky.dds").index == 4);

    // The manifest reads back, and pack puts the new files into their archives.
    Manifest reread;
    REQUIRE_MESSAGE(reread.Read(ManifestPath(f.clientData), error), error);
    CHECK(reread.files.size() == m.files.size());
    Status status;
    REQUIRE_MESSAGE(GetStatus(f.clientData, 2, status, error), error);
    CHECK(status.untracked.empty());
    CHECK(status.added.size() == 2);
    CHECK(status.changed == std::vector<std::string>{"Floor.dds"});

    const PackStats packed = f.DoPack();
    CHECK(packed.added == 2);
    CHECK(packed.untracked == 0);
    CHECK(EntryNames(f.out / "a.kom") == std::vector<std::string>{"DLG_Main.lua", "DLG_Other.lua", "Wall.dds"});
    CHECK(EntryNames(f.out / "b.kom") == std::vector<std::string>{"Miss.ogg", "Floor.dds", "Jump.ogg", "Sky.dds"});
    x2::kom::Mount mount([](const std::string&) {});
    REQUIRE_MESSAGE(mount.AddFolder(f.out, error), error);
    Bytes jump;
    REQUIRE(mount.Read("Jump.ogg", jump, error));
    CHECK(jump == newSound);
}

TEST_CASE("assets: the manager adds, moves and removes files on disk")
{
    Fixture f;
    TempDir outside;
    WriteFile(outside / "Extra.dds", Pattern(50, 11));
    ClientData data = f.Open();
    std::string error;
    REQUIRE_MESSAGE(data.AddFile(outside / "Extra.dds", "ui/textures/extra", error), error);
    WriteFile(outside / "Wall.dds", Pattern(50, 12));
    CHECK_FALSE(data.AddFile(outside / "Wall.dds", "", error));  // a name client_data already has
    CHECK_FALSE(data.AddFile(outside / "Extra.dds", "/abs", error));
    REQUIRE_MESSAGE(data.Rescan(2, error), error);
    CHECK(data.Find("Extra.dds")->folder == "ui/textures/extra");
    CHECK(data.ArchiveOf(*data.Find("Extra.dds")) == "a.kom");

    REQUIRE_MESSAGE(data.MoveFile("Hit.ogg", "audio/hits", error), error);
    REQUIRE_MESSAGE(data.RemoveFile("Extra.dds", error), error);
    CHECK_FALSE(data.MoveFile("Nothing.ogg", "x", error));
    REQUIRE_MESSAGE(data.Rescan(2, error), error);
    CHECK(data.Find("Hit.ogg")->folder == "audio/hits");
    CHECK(StateOf(data, "Hit.ogg") == FileState::Unchanged);
    CHECK(data.Find("Extra.dds") == nullptr);
}
