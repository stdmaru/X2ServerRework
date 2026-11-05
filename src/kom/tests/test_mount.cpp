#include "helpers.h"

#include "x2/kom/mount.h"
#include "x2/kom/writer.h"

using namespace x2::kom;
using namespace kom_test;

namespace {

void Build(const std::filesystem::path& path, const std::vector<std::pair<std::string, std::string>>& files)
{
    ArchiveBuilder b;
    std::string error;
    for (const auto& [name, text] : files)
        REQUIRE(b.Add(name, kZlib, ToBytes(text), error));
    REQUIRE_MESSAGE(b.Write(path, error), error);
}

std::string Name(const std::filesystem::path& path)
{
    return path.filename().string();
}

} // namespace

TEST_CASE("kom: load order is the client's (data001..data150, then datai001..)")
{
    TempDir dir;
    for (const char* name : {"datai001.kom", "data150.kom", "DATA002.KOM", "data001.kom", "readme.txt"})
        WriteFile(dir / name, Bytes{});
    std::filesystem::create_directory(dir / "sub.kom");
    std::vector<std::filesystem::path> paths;
    std::string error;
    REQUIRE(ArchivesInLoadOrder(dir.Path(), paths, error));
    std::vector<std::string> names;
    for (const auto& p : paths)
        names.push_back(Name(p));
    CHECK(names == std::vector<std::string>{"data001.kom", "DATA002.KOM", "data150.kom", "datai001.kom"});

    CHECK_FALSE(ArchivesInLoadOrder(dir / "missing", paths, error));
}

TEST_CASE("kom: a name in more than one archive: the first one read wins, with a warning")
{
    TempDir dir;
    Build(dir / "data001.kom", {{"Common.lua", "from 1"}, {"One.lua", "one"}});
    Build(dir / "data002.kom", {{"COMMON.LUA", "from 2"}, {"Two.lua", "two"}});
    Build(dir / "datai001.kom", {{"Three.lua", "three"}});

    std::vector<std::string> warnings;
    Mount mount([&](const std::string& w) { warnings.push_back(w); });
    std::string error;
    REQUIRE_MESSAGE(mount.AddFolder(dir.Path(), error), error);

    CHECK(mount.Archives().size() == 3);
    CHECK(mount.NameCount() == 4);
    REQUIRE(warnings.size() == 1);
    CHECK(warnings[0] == "COMMON.LUA is in both data001.kom and data002.kom; using data001.kom");

    Bytes out;
    REQUIRE(mount.Read("common.lua", out, error));
    CHECK(out == ToBytes("from 1"));
    REQUIRE(mount.Read("THREE.lua", out, error));
    CHECK(out == ToBytes("three"));
    const Mount::Hit hit = mount.Find("two.lua");
    REQUIRE(hit);
    CHECK(Name(hit.archive->Path()) == "data002.kom");
    CHECK_FALSE(mount.Find("none.lua"));
    CHECK_FALSE(mount.Read("none.lua", out, error));
}

TEST_CASE("kom: a name repeated inside one archive keeps its first entry")
{
    const Bytes a = Encoded("x.lua", kZlib, ToBytes("first")), b = Encoded("X.LUA", kZlib, ToBytes("second"));
    char xml[512];
    std::snprintf(xml, sizeof xml,
                  "<Files><File Name=\"x.lua\" Size=\"5\" CompressedSize=\"%zu\" Checksum=\"%08x\" />"
                  "<File Name=\"X.LUA\" Size=\"6\" CompressedSize=\"%zu\" Checksum=\"%08x\" /></Files>",
                  a.size(), Adler32(a), b.size(), Adler32(b));
    TempDir dir;
    WriteFile(dir / "dup.kom", RawArchive(xml, {a, b}));

    std::vector<std::string> warnings;
    Mount mount([&](const std::string& w) { warnings.push_back(w); });
    std::string error;
    REQUIRE(mount.AddArchive(dir / "dup.kom", error));
    REQUIRE(warnings.size() == 1);
    CHECK(warnings[0] == "X.LUA appears twice in dup.kom; the first entry is used");
    Bytes out;
    REQUIRE(mount.Read("x.lua", out, error));
    CHECK(out == ToBytes("first"));

    CHECK_FALSE(mount.AddArchive(dir / "missing.kom", error));
    CHECK(mount.Archives().size() == 1);
}
