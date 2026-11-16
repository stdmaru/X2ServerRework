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
    REQUIRE_MESSAGE(data.Sync(stats, error), error);
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

TEST_CASE("assets: ignored files stay out of the manifest and the archives")
{
    Fixture f;
    WriteFile(f.clientData / "layout.txt", ToBytes("notes"));
    WriteFile(f.clientData / "tools" / "x2_assets", ToBytes("binary"));
    WriteFile(f.clientData / "sound" / "Jump.ogg", Pattern(100, 13));

    IgnoreList list;
    list.patterns = {"layout.txt", "tools/", "sound/*.wav", "*.TMP"};
    CHECK(list.Matches("layout.txt"));
    CHECK(list.Matches("deep/LAYOUT.txt"));  // a name pattern, in any folder
    CHECK(list.Matches("tools/x2_assets"));
    CHECK(list.Matches("tools/sub/a.bin"));
    CHECK(list.Matches("sound/a.wav"));
    CHECK_FALSE(list.Matches("other/sound/a.wav"));
    CHECK(list.Matches("x/y.tmp"));
    CHECK_FALSE(list.Matches("toolsx/a"));

    ClientData data = f.Open();
    std::string error;
    REQUIRE_MESSAGE(data.Ignore("layout.txt", error), error);
    REQUIRE_MESSAGE(data.IgnoreFolder("tools", error), error);
    CHECK_FALSE(data.Ignore("Wall.dds", error));  // in the manifest
    REQUIRE_MESSAGE(data.Ignore("notes.md", error), error);  // not there yet: by name
    WriteFile(f.clientData / "deep" / "notes.md", Pattern(10, 3));
    REQUIRE_MESSAGE(data.Rescan(2, error), error);
    CHECK(StateOf(data, "layout.txt") == FileState::Ignored);
    CHECK(StateOf(data, "x2_assets") == FileState::Ignored);
    CHECK(StateOf(data, "Jump.ogg") == FileState::Untracked);
    CHECK(StateOf(data, "notes.md") == FileState::Ignored);
    CHECK_FALSE(data.SetArchive("layout.txt", "a.kom", error));
    CHECK_FALSE(data.Unignore("x2_assets", error));  // its folder's pattern
    CHECK(error.find("tools/") != std::string::npos);

    Status status;
    REQUIRE_MESSAGE(GetStatus(f.clientData, 2, status, error), error);
    CHECK(status.ignored.size() == 3);
    REQUIRE(status.untracked.size() == 1);

    SyncStats stats;
    REQUIRE_MESSAGE(data.Sync(stats, error), error);
    CHECK(stats.added == 1);  // Jump.ogg only
    CHECK(StateOf(data, "layout.txt") == FileState::Ignored);
    const PackStats packed = f.DoPack();
    CHECK(packed.ignored == 3);
    CHECK(packed.untracked == 0);

    REQUIRE_MESSAGE(data.Unignore("layout.txt", error), error);
    REQUIRE_MESSAGE(data.Rescan(2, error), error);
    CHECK(StateOf(data, "layout.txt") == FileState::Untracked);
}

TEST_CASE("assets: a sync creates new archives, and pack writes them")
{
    Fixture f;
    WriteFile(f.clientData / "mods" / "Mod.dds", Pattern(300, 14));
    ClientData data = f.Open();
    std::string error;
    CHECK_FALSE(data.CreateArchive("a.KOM", error));
    CHECK_FALSE(data.CreateArchive("bad/name.kom", error));
    CHECK_FALSE(data.CreateArchive("data.txt", error));
    CHECK_FALSE(data.SetArchive("Mod.dds", "aa.kom", error));
    REQUIRE_MESSAGE(data.CreateArchive("aa.kom", error), error);
    CHECK(data.Archives() == std::vector<std::string>{"a.kom", "b.kom", "aa.kom"});
    CHECK(data.EditCount() == 1);
    REQUIRE_MESSAGE(data.SetArchive("Mod.dds", "aa.kom", error), error);
    REQUIRE_MESSAGE(data.SetArchive("Hit.ogg", "aa.kom", error), error);

    SyncStats stats;
    REQUIRE_MESSAGE(data.Sync(stats, error), error);
    CHECK(stats.archives == 1);
    CHECK(data.Archives() == std::vector<std::string>{"a.kom", "aa.kom", "b.kom"});  // load order: by name
    CHECK(data.GetManifest().archives[1].count == 2);

    f.DoPack();
    CHECK(EntryNames(f.out / "aa.kom") == std::vector<std::string>{"Hit.ogg", "Mod.dds"});
    CHECK(EntryNames(f.out / "b.kom") == std::vector<std::string>{"Miss.ogg"});
}

TEST_CASE("assets: pack can rewrite only the archives that changed")
{
    Fixture f;
    PackStats stats = f.DoPack();  // writes the state of what it wrote
    CHECK(stats.skipped == 0);
    REQUIRE(fs::exists(PackStatePath(f.out)));

    PackOptions options;
    options.clientData = f.clientData;
    options.destination = f.out;
    options.jobs = 2;
    options.onlyChanged = true;
    std::string error;
    REQUIRE_MESSAGE(Pack(options, stats, error), error);
    CHECK(stats.skipped == 2);

    const auto bTime = fs::last_write_time(f.out / "b.kom");
    WriteFile(f.clientData / "ui" / "textures" / "Wall.dds", Pattern(77, 15));
    REQUIRE_MESSAGE(Pack(options, stats, error), error);
    CHECK(stats.skipped == 1);  // b.kom
    CHECK(stats.changed == 1);
    CHECK(fs::last_write_time(f.out / "b.kom") == bTime);

    // An archive changed or removed in the destination is written again.
    fs::remove(f.out / "b.kom");
    REQUIRE_MESSAGE(Pack(options, stats, error), error);
    CHECK(stats.skipped == 1);  // a.kom now
    CHECK(fs::exists(f.out / "b.kom"));
}

TEST_CASE("assets: a commit makes client_data's content the original, and a plain pack does not")
{
    Fixture f;
    const Bytes wall = Pattern(4321, 16);
    WriteFile(f.clientData / "ui" / "textures" / "Wall.dds", wall);
    WriteFile(f.clientData / "sound" / "Jump.ogg", Pattern(222, 17));
    fs::remove(f.clientData / "sound" / "Hit.ogg");
    {
        ClientData data = f.Open();
        SyncStats synced;
        std::string error;
        REQUIRE_MESSAGE(data.Sync(synced, error), error);  // adds Jump.ogg, drops Hit.ogg
    }
    WriteFile(f.clientData / "sound" / "Miss.ogg", Pattern(333, 18));  // edited after the sync
    fs::remove(f.clientData / "ui" / "scripts" / "DLG_Other.lua");    // deleted after the sync

    const PackStats stats = f.DoPack();
    CHECK(stats.committed == 0);
    CHECK(stats.added == 1);
    CHECK(StateOf(f.Open(), "Miss.ogg") == FileState::Modified);  // a pack only writes the archives
    const Bytes a0 = ReadFile(f.out / "a.kom"), b0 = ReadFile(f.out / "b.kom");

    ClientData::CommitStats committed;
    std::string error;
    ClientData data = f.Open();
    REQUIRE_MESSAGE(data.Commit(2, committed, error), error);
    CHECK(committed.sync.removed == 1);  // DLG_Other.lua, by the commit's sync
    CHECK(committed.committed == 3);     // Wall.dds and Miss.ogg edited, Jump.ogg new
    for (const ClientDataFile& file : data.Files())
    {
        CAPTURE(file.name);
        CHECK(file.state == FileState::Unchanged);
    }
    ClientData after = f.Open();
    for (const ClientDataFile& file : after.Files())
    {
        CAPTURE(file.name);
        CHECK(file.state == FileState::Unchanged);
    }

    // Packing the committed manifest gives the same archives as packing the changes did.
    fs::remove_all(f.out);
    f.DoPack();
    CHECK(ReadFile(f.out / "a.kom") == a0);
    CHECK(ReadFile(f.out / "b.kom") == b0);
}

TEST_CASE("assets: moving, renaming and trashing update the model without a rescan")
{
    Fixture f;
    ClientData data = f.Open();
    std::string error;
    const std::string wallHash = data.Find("Wall.dds")->sha256;

    REQUIRE_MESSAGE(data.MoveFile("Wall.dds", "art/walls", error), error);
    CHECK(data.Find("Wall.dds")->folder == "art/walls");
    CHECK(data.Find("Wall.dds")->sha256 == wallHash);
    CHECK(data.Folders().contains("art"));
    CHECK(fs::exists(f.clientData / "art" / "walls" / "Wall.dds"));
    CHECK_FALSE(data.MoveFile("Wall.dds", "", error, "Floor.dds"));  // another file's name

    REQUIRE_MESSAGE(data.MoveFile("Wall.dds", "art/walls", error, "Wall2.dds"), error);  // rename
    CHECK(StateOf(data, "Wall.dds") == FileState::Deleted);  // the record's name is gone
    CHECK(StateOf(data, "Wall2.dds") == FileState::Untracked);
    REQUIRE_MESSAGE(data.MoveFile("Wall2.dds", "art/walls", error, "Wall.dds"), error);
    CHECK(StateOf(data, "Wall.dds") == FileState::Unchanged);

    REQUIRE_MESSAGE(data.MoveFolder("art", "ui", error), error);
    CHECK(data.Find("Wall.dds")->folder == "ui/art/walls");
    CHECK(data.Folders().contains("ui/art/walls"));
    CHECK_FALSE(data.Folders().contains("art"));
    CHECK_FALSE(data.MoveFolder("ui", "ui/art", error));  // into itself
    REQUIRE_MESSAGE(data.MoveFolder("ui/art", "ui", error, "pictures"), error);
    CHECK(data.Find("Wall.dds")->folder == "ui/pictures/walls");

    REQUIRE_MESSAGE(data.CreateFolder("empty/one", error), error);
    CHECK(data.Folders().contains("empty/one"));
    REQUIRE_MESSAGE(data.RemoveEmptyFolder("empty/one", error), error);
    CHECK_FALSE(data.Folders().contains("empty/one"));

    const fs::path trash = f.clientData / ".kom" / "trash" / "test";
    ClientData::Trashed folder, file;
    REQUIRE_MESSAGE(data.Trash("ui/pictures", true, trash, folder, error), error);
    CHECK(StateOf(data, "Wall.dds") == FileState::Deleted);
    CHECK(folder.files.size() == 1);
    REQUIRE_MESSAGE(data.Trash("sound/Hit.ogg", false, trash, file, error), error);
    CHECK(StateOf(data, "Hit.ogg") == FileState::Deleted);
    CHECK(fs::exists(trash / "sound" / "Hit.ogg"));

    REQUIRE_MESSAGE(data.Restore(folder, trash, error), error);
    REQUIRE_MESSAGE(data.Restore(file, trash, error), error);
    CHECK(StateOf(data, "Wall.dds") == FileState::Unchanged);
    CHECK(StateOf(data, "Hit.ogg") == FileState::Unchanged);
    CHECK(data.Locate("Wall.dds") == f.clientData / "ui" / "pictures" / "walls" / "Wall.dds");

    // What the model says is what a fresh read finds.
    ClientData fresh = f.Open();
    for (const ClientDataFile& file2 : data.Files())
    {
        CAPTURE(file2.name);
        const ClientDataFile* g = fresh.Find(file2.name);
        REQUIRE(g);
        CHECK(g->folder == file2.folder);
        CHECK(g->state == file2.state);
    }
}
