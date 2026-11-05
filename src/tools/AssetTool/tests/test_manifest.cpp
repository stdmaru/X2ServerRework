#include "helpers.h"

#include "x2/assets/manifest.h"

using namespace x2::assets;
using namespace assets_test;

TEST_CASE("assets: manifest round trip")
{
    TempDir dir;
    Manifest m;
    m.archives = {{"data001.kom", 2}, {"datai001.kom", 1}};
    FileRecord a;
    a.name = "DLG_Main.lua";
    a.archive = "data001.kom";
    a.index = 0;
    a.algorithm = 2;
    a.size = 1234;
    a.storedSize = 567;
    a.checksum = 0x0badf00d;
    a.fileTime = 0;
    a.encoding = Encoding::Zlib;
    a.sha256 = std::string(64, 'a');
    FileRecord b = a;
    b.name = "Ünïcode name.tga";
    b.index = 1;
    b.encoding = Encoding::Blob;
    b.fileTime = 0xffffffff;
    FileRecord c = a;
    c.name = "opaque.bin";
    c.archive = "datai001.kom";
    c.encoding = Encoding::Stored;
    m.files = {a, b, c};

    const auto path = dir / "manifest.tsv";
    std::string error;
    REQUIRE_MESSAGE(m.Write(path, error), error);
    Manifest r;
    REQUIRE_MESSAGE(r.Read(path, error), error);
    REQUIRE(r.archives.size() == 2);
    CHECK(r.archives[1].name == "datai001.kom");
    CHECK(r.archives[1].count == 1);
    REQUIRE(r.files.size() == 3);
    CHECK(r.files[1].name == "Ünïcode name.tga");
    CHECK(r.files[1].encoding == Encoding::Blob);
    CHECK(r.files[1].fileTime == 0xffffffffu);
    CHECK(r.files[0].checksum == 0x0badf00du);
    CHECK(r.files[0].algorithm == 2);
    CHECK(r.files[0].size == 1234);
    CHECK(r.files[0].storedSize == 567);
    CHECK(r.files[2].encoding == Encoding::Stored);

    // Writing what was read gives the same bytes.
    const Bytes first = ReadFile(path);
    REQUIRE(r.Write(path, error));
    CHECK(ReadFile(path) == first);
    CHECK_FALSE(std::filesystem::exists(dir / "manifest.tsv.tmp"));
}

TEST_CASE("assets: bad manifests")
{
    TempDir dir;
    const auto path = dir / "manifest.tsv";
    const std::string sha(64, '0');
    const std::string good = "file\ta.tga\tdata001.kom\t0\t0\t10\t8\t00000001\t00000001\tzlib\t" + sha + "\n";
    Manifest m;
    std::string error;

    WriteFile(path, ToBytes("x2-assets-manifest\t1\n" + good));
    CHECK_MESSAGE(m.Read(path, error), error);

    const auto rejects = [&](const std::string& text) {
        WriteFile(path, ToBytes(text));
        return !m.Read(path, error);
    };
    CHECK(rejects(""));
    CHECK(rejects(good));                                              // no version line
    CHECK(rejects("x2-assets-manifest\t2\n"));                         // future version
    CHECK(rejects("x2-assets-manifest\t1\nfolder\tx\n"));              // unknown record
    CHECK(rejects("x2-assets-manifest\t1\narchive\tdata001.kom\n"));   // missing count
    CHECK(rejects("x2-assets-manifest\t1\n" + std::string(good).replace(good.find("zlib"), 4, "lzma")));
    CHECK(rejects("x2-assets-manifest\t1\n" + std::string(good).replace(good.find("00000001"), 8, "1")));
    CHECK(rejects("x2-assets-manifest\t1\n" + good.substr(0, good.size() - 2) + "\n"));  // short sha256
    CHECK(rejects("x2-assets-manifest\t1\n" + good.substr(0, good.size() - 1) + "\textra\n"));
    CHECK(error.find(":2:") != std::string::npos);
    CHECK_FALSE(m.Read(dir / "missing.tsv", error));
}
