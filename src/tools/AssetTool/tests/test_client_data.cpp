// Import and pack on archives built here: both XML header layouts, every encoding a manifest records.
#include "helpers.h"

#include "x2/assets/client_data.h"

#include <cstdio>

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

    Bytes lua = Pattern(5000, 1);
    Bytes wall = Pattern(70000, 2);
    Bytes big = Pattern(300000, 3);
    Bytes level1 = Pattern(40000, 4);
    Bytes opaque = Pattern(321, 5);
    Bytes unicode = Pattern(1000, 6);
    Bytes hit = Pattern(9000, 7);
    Bytes miss = Pattern(8000, 8);

    Fixture()
    {
        fs::create_directories(source);
        std::string error;

        // a.kom: KOG's layout, KOG's zlib level; one blob at another level, one opaque entry.
        x2::kom::ArchiveBuilder a;
        a.SetLevel(x2::kom::kKogLevel);
        REQUIRE(a.Add("DLG_Main.lua", x2::kom::kXorCrc, lua, error));
        REQUIRE(a.Add("Wall.dds", x2::kom::kZlib, wall, error));
        REQUIRE(a.Add("Big.tga", x2::kom::kZlib, big, error));
        REQUIRE(a.AddStored("Level1.tga", x2::kom::kZlib, static_cast<std::uint32_t>(level1.size()),
                            Encoded("Level1.tga", x2::kom::kZlib, level1, 1), error));
        REQUIRE(a.AddStored("opaque.bin", x2::kom::kBlowfish, 999, opaque, error, 0x1234));
        REQUIRE(a.Add("Ünï.ogg", x2::kom::kZlib, unicode, error));
        REQUIRE_MESSAGE(a.Write(source / "a.kom", error), error);

        // b.kom: the other packer's layout (alphabetical attributes, FileTime 0).
        const Bytes hitBlob = Encoded("Hit.ogg", x2::kom::kZlib, hit, x2::kom::kKogLevel);
        const Bytes missBlob = Encoded("Miss.ogg", x2::kom::kZlib, miss, x2::kom::kKogLevel);
        char xml[512];
        std::snprintf(xml, sizeof xml,
                      "<?xml version=\"1.0\" ?><Files>"
                      "<File Algorithm=\"0\" Checksum=\"%08x\" CompressedSize=\"%zu\" FileTime=\"00000000\" "
                      "Name=\"Hit.ogg\" Size=\"%zu\"/>"
                      "<File Algorithm=\"0\" Checksum=\"%08x\" CompressedSize=\"%zu\" FileTime=\"00000000\" "
                      "Name=\"Miss.ogg\" Size=\"%zu\"/></Files>",
                      x2::kom::Adler32(hitBlob), hitBlob.size(), hit.size(), x2::kom::Adler32(missBlob),
                      missBlob.size(), miss.size());
        WriteFile(source / "b.kom", RawArchive(xml, {hitBlob, missBlob}, 0));
    }

    Layout Rules()
    {
        Layout layout;
        std::string error;
        REQUIRE(layout.Parse("ui            *.lua\n"
                             "ui/textures   *.dds\n"
                             "sound/{archive} *.ogg  b.kom\n",
                             error));
        return layout;
    }

    ImportStats DoImport()
    {
        ImportOptions options;
        options.source = source;
        options.destination = clientData;
        options.layout = Rules();
        options.jobs = 2;
        ImportStats stats;
        std::string error;
        REQUIRE_MESSAGE(Import(options, stats, error), error);
        return stats;
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

    Status DoStatus()
    {
        Status status;
        std::string error;
        REQUIRE_MESSAGE(GetStatus(clientData, 2, status, error), error);
        return status;
    }

    void CheckIdentical(const char* archive)
    {
        CAPTURE(archive);
        CHECK(ReadFile(out / archive) == ReadFile(source / archive));
    }
};

Bytes ReadEntry(const fs::path& archivePath, const std::string& name)
{
    std::string error;
    auto archive = x2::kom::Archive::Open(archivePath, error);
    REQUIRE_MESSAGE(archive, error);
    const x2::kom::Entry* e = archive->Find(name);
    REQUIRE_MESSAGE(e, name);
    Bytes data;
    REQUIRE_MESSAGE(archive->Read(*e, data, error), error);
    return data;
}

} // namespace

TEST_CASE("assets: import lays files out by the rules and records how to repack them")
{
    Fixture f;
    const ImportStats stats = f.DoImport();
    CHECK(stats.archives == 2);
    CHECK(stats.files == 8);
    CHECK(stats.keptBlobs == 1);  // Level1.tga
    CHECK(stats.stored == 1);     // opaque.bin

    CHECK(ReadFile(f.clientData / "ui" / "DLG_Main.lua") == f.lua);
    CHECK(ReadFile(f.clientData / "ui" / "textures" / "Wall.dds") == f.wall);
    CHECK(ReadFile(f.clientData / "sound" / "b" / "Hit.ogg") == f.hit);
    CHECK(ReadFile(f.clientData / "_unsorted" / "a" / "Big.tga") == f.big);
    CHECK(ReadFile(f.clientData / "_unsorted" / "a" / "opaque.bin") == f.opaque);       // the stored blob
    CHECK(ReadFile(f.clientData / "_unsorted" / "a" / PathFromUtf8("Ünï.ogg")) == f.unicode);
    CHECK(fs::exists(BlobPath(f.clientData, "Level1.tga")));
    CHECK_FALSE(fs::exists(BlobPath(f.clientData, "Big.tga")));

    Manifest m;
    std::string error;
    REQUIRE_MESSAGE(m.Read(ManifestPath(f.clientData), error), error);
    REQUIRE(m.archives.size() == 2);
    CHECK(m.archives[0].name == "a.kom");
    CHECK(m.archives[0].count == 6);
    REQUIRE(m.files.size() == 8);
    CHECK(m.files[0].name == "DLG_Main.lua");
    CHECK(m.files[0].algorithm == x2::kom::kXorCrc);
    CHECK(m.files[0].encoding == Encoding::Zlib);
    CHECK(m.files[3].encoding == Encoding::Blob);
    CHECK(m.files[4].encoding == Encoding::Stored);
    CHECK(m.files[4].size == 999);
    CHECK(m.files[4].fileTime == 0x1234u);
    CHECK(m.files[7].name == "Miss.ogg");
    CHECK(m.files[7].index == 1);
    CHECK(m.files[7].fileTime == 0);

    CHECK(f.DoStatus().changed.empty());
}

TEST_CASE("assets: an unchanged client_data packs back byte for byte, wherever its files are")
{
    Fixture f;
    f.DoImport();
    PackStats stats = f.DoPack();
    CHECK(stats.archives == 2);
    CHECK(stats.identicalHeaders == 2);
    CHECK(stats.files == 8);
    CHECK(stats.changed == 0);
    CHECK(stats.notReproduced == 0);
    f.CheckIdentical("a.kom");
    f.CheckIdentical("b.kom");

    // The manifest is keyed by name: moving files around changes nothing.
    fs::create_directories(f.clientData / "elsewhere" / "deep");
    fs::rename(f.clientData / "ui" / "DLG_Main.lua", f.clientData / "elsewhere" / "deep" / "DLG_Main.lua");
    fs::rename(f.clientData / "sound", f.clientData / "audio");
    fs::remove_all(f.out);
    stats = f.DoPack();
    CHECK(stats.identicalHeaders == 2);
    f.CheckIdentical("a.kom");
    f.CheckIdentical("b.kom");
    const Status status = f.DoStatus();
    CHECK(status.changed.empty());
    CHECK(status.missing.empty());
    CHECK(status.added.empty());
}

TEST_CASE("assets: edited, missing and new files")
{
    Fixture f;
    f.DoImport();

    const Bytes wall2 = Pattern(12345, 42);
    const Bytes level2 = Pattern(2222, 43);
    WriteFile(f.clientData / "ui" / "textures" / "Wall.dds", wall2);
    WriteFile(f.clientData / "_unsorted" / "a" / "Level1.tga", level2);  // a kept blob no longer applies
    fs::remove(f.clientData / "sound" / "b" / "Hit.ogg");
    WriteFile(f.clientData / "new" / "New.tga", Pattern(10, 44));

    const Status status = f.DoStatus();
    CHECK(status.changed == std::vector<std::string>{"Wall.dds", "Level1.tga"});
    CHECK(status.missing == std::vector<std::string>{"Hit.ogg"});
    REQUIRE(status.added.size() == 1);
    CHECK(status.added[0].filename() == "New.tga");

    const PackStats stats = f.DoPack();
    CHECK(stats.changed == 2);
    CHECK(stats.missing == 1);
    CHECK(stats.added == 1);
    CHECK(stats.identicalHeaders == 0);
    CHECK(stats.files == 7);

    CHECK(ReadEntry(f.out / "a.kom", "Wall.dds") == wall2);
    CHECK(ReadEntry(f.out / "a.kom", "Level1.tga") == level2);
    CHECK(ReadEntry(f.out / "a.kom", "DLG_Main.lua") == f.lua);
    CHECK(ReadEntry(f.out / "a.kom", "Ünï.ogg") == f.unicode);
    std::string error;
    auto b = x2::kom::Archive::Open(f.out / "b.kom", error);
    REQUIRE_MESSAGE(b, error);
    REQUIRE(b->Entries().size() == 1);
    CHECK(b->Entries()[0].name == "Miss.ogg");
    b.reset();  // Windows refuses to replace an archive that is open

    // Opaque entries cannot be re-encoded.
    WriteFile(f.clientData / "_unsorted" / "a" / "opaque.bin", Pattern(5, 45));
    PackOptions options;
    options.clientData = f.clientData;
    options.destination = f.out;
    PackStats ignored;
    CHECK_FALSE(Pack(options, ignored, error));
    CHECK_MESSAGE(error.find("opaque.bin") != std::string::npos, error);
}

TEST_CASE("assets: import and pack refuse what a name-keyed manifest cannot hold")
{
    Fixture f;
    std::string error;
    ImportOptions options;
    options.source = f.source;
    options.destination = f.clientData;
    ImportStats stats;

    SUBCASE("a destination that is not empty")
    {
        WriteFile(f.clientData / "x.txt", ToBytes("x"));
        CHECK_FALSE(Import(options, stats, error));
        CHECK(error.find("not empty") != std::string::npos);
    }
    SUBCASE("a name in two archives")
    {
        x2::kom::ArchiveBuilder c;
        REQUIRE(c.Add("wall.DDS", x2::kom::kZlib, f.wall, error));
        REQUIRE(c.Write(f.source / "c.kom", error));
        CHECK_FALSE(Import(options, stats, error));
        CHECK(error.find("wall.DDS (a.kom, c.kom)") != std::string::npos);
        CHECK_FALSE(fs::exists(f.clientData));
    }
    SUBCASE("a name in two folders of client_data")
    {
        REQUIRE(Import(options, stats, error));
        WriteFile(f.clientData / "copy" / "WALL.dds", f.wall);
        PackOptions pack;
        pack.clientData = f.clientData;
        pack.destination = f.out;
        PackStats packStats;
        CHECK_FALSE(Pack(pack, packStats, error));
        CHECK(error.find("two folders") != std::string::npos);
    }
    SUBCASE("no archives")
    {
        options.source = f.dir / "nothing";
        fs::create_directories(options.source);
        CHECK_FALSE(Import(options, stats, error));
    }
}
