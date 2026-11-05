#include "helpers.h"

#include "x2/kom/archive.h"
#include "x2/kom/writer.h"
#include "x2/platform/file.h"

#include <thread>

using namespace x2::kom;
using namespace kom_test;

namespace {

std::shared_ptr<Archive> OpenOrFail(const std::filesystem::path& path)
{
    std::string error;
    std::shared_ptr<Archive> archive = Archive::Open(path, error);
    REQUIRE_MESSAGE(archive, error);
    return archive;
}

std::string OpenError(const Bytes& raw)
{
    TempDir dir;
    WriteFile(dir / "bad.kom", raw);
    std::string error;
    CHECK_FALSE(Archive::Open(dir / "bad.kom", error));
    return error;
}

// data001-style archive with every algorithm, an opaque algorithm 1 entry and an empty file.
void BuildSample(const std::filesystem::path& path)
{
    ArchiveBuilder b;
    std::string error;
    REQUIRE(b.Add("DLG_Sample.lua", kXorCrc, Pattern(3000, 1), error));
    REQUIRE(b.Add("Texture_A.dds", kZlib, Pattern(70000, 2), error));
    REQUIRE(b.Add("Later.lua", kXorCrc3, Pattern(1234, 3), error));
    REQUIRE(b.Add("Empty.txt", kZlib, Bytes{}, error));
    REQUIRE(b.AddStored("Old.bin", kBlowfish, 999, Pattern(77, 4), error, 0x12345678u));
    REQUIRE_MESSAGE(b.Write(path, error), error);
}

} // namespace

TEST_CASE("kom: write, list from the header and read every algorithm")
{
    TempDir dir;
    BuildSample(dir / "data001.kom");
    const auto a = OpenOrFail(dir / "data001.kom");

    const auto& entries = a->Entries();
    REQUIRE(entries.size() == 5);
    CHECK(entries[0].name == "DLG_Sample.lua");
    CHECK(entries[0].algorithm == kXorCrc);
    CHECK(entries[0].size == 3000);
    CHECK(entries[0].fileTime == entries[0].checksum);  // KOG's packer stores the checksum as the time
    CHECK(entries[0].offset == a->RawHeader().size());
    CHECK(entries[1].offset == entries[0].offset + entries[0].storedSize);
    CHECK(entries[4].fileTime == 0x12345678u);
    CHECK(entries[4].size == 999);

    std::uint32_t sum = 0;
    for (const Entry& e : entries)
        sum += e.fileTime;
    CHECK(a->FileTime() == sum);

    const std::string xml(a->RawHeader().begin() + kFixedHeaderSize, a->RawHeader().end());
    CHECK(xml.starts_with("<?xml version=\"1.0\"?><Files><File Name=\"DLG_Sample.lua\" Size=\"3000\" CompressedSize=\""));
    CHECK(xml.ends_with(" Algorithm=\"1\" /></Files>"));

    std::string error;
    Bytes out;
    const unsigned seeds[] = {1, 2, 3};
    for (int i = 0; i < 3; ++i)
    {
        REQUIRE_MESSAGE(a->Read(entries[static_cast<std::size_t>(i)], out, error), error);
        CHECK(out == Pattern(entries[static_cast<std::size_t>(i)].size, seeds[i]));
    }
    REQUIRE(a->Read(entries[3], out, error));
    CHECK(out.empty());

    // Algorithm 1 stays opaque: its stored blob can be read, not decoded.
    CHECK_FALSE(a->Read(entries[4], out, error));
    REQUIRE(a->ReadStored(entries[4], out, error));
    CHECK(out == Pattern(77, 4));

    // Lookups are case-insensitive (ASCII), as the client's.
    REQUIRE(a->Find("dlg_sample.LUA"));
    CHECK(a->Find("dlg_sample.LUA") == &entries[0]);
    CHECK_FALSE(a->Find("missing.lua"));
}

TEST_CASE("kom: an archive rewritten with no changes is byte-identical")
{
    TempDir dir;
    BuildSample(dir / "a.kom");
    const auto a = OpenOrFail(dir / "a.kom");
    std::string error;
    REQUIRE_MESSAGE(ArchiveBuilder::From(a).Write(dir / "b.kom", error), error);
    CHECK(ReadFile(dir / "b.kom") == ReadFile(dir / "a.kom"));
    CHECK(a->IsOpen());  // another file: the source stays open
}

TEST_CASE("kom: the other packer's header layout is read, kept when unchanged and regenerated on change")
{
    // The layout of 58 of the client's 173 archives (2026-11-05): `<?xml version="1.0" ?>`, attributes in
    // alphabetical order, no space before "/>", FileTime 0.
    const Bytes data1 = Pattern(500, 7), data2 = Pattern(20, 8);
    const Bytes blob1 = Encoded("11Gang_Lighting_01.Y", kZlib, data1), blob2 = Encoded("Z.lua", kXorCrc, data2);
    char xml[512];
    std::snprintf(xml, sizeof xml,
                  "<?xml version=\"1.0\" ?><Files>"
                  "<File Algorithm=\"0\" Checksum=\"%08x\" CompressedSize=\"%zu\" FileTime=\"00000000\" "
                  "Name=\"11Gang_Lighting_01.Y\" Size=\"500\"/>"
                  "<File Algorithm=\"2\" Checksum=\"%08x\" CompressedSize=\"%zu\" FileTime=\"00000000\" "
                  "Name=\"Z.lua\" Size=\"20\"/></Files>",
                  Adler32(blob1), blob1.size(), Adler32(blob2), blob2.size());
    const Bytes raw = RawArchive(xml, {blob1, blob2});

    TempDir dir;
    WriteFile(dir / "data002.kom", raw);
    const auto a = OpenOrFail(dir / "data002.kom");
    REQUIRE(a->Entries().size() == 2);
    CHECK(a->Entries()[0].fileTime == 0);
    std::string error;
    Bytes out;
    REQUIRE(a->Read(a->Entries()[1], out, error));
    CHECK(out == data2);

    REQUIRE(ArchiveBuilder::From(a).Write(dir / "same.kom", error));
    CHECK(ReadFile(dir / "same.kom") == raw);

    ArchiveBuilder changed = ArchiveBuilder::From(a);
    REQUIRE(changed.Add("New.txt", kZlib, ToBytes("hello"), error));
    REQUIRE(changed.Write(dir / "changed.kom", error));
    const auto c = OpenOrFail(dir / "changed.kom");
    const std::string cxml(c->RawHeader().begin() + kFixedHeaderSize, c->RawHeader().end());
    CHECK(cxml.starts_with("<?xml version=\"1.0\"?><Files><File Name=\"11Gang_Lighting_01.Y\" Size=\"500\""));
    CHECK(c->Entries()[0].fileTime == 0);  // copied entries keep their header values
    CHECK(c->Entries()[0].checksum == a->Entries()[0].checksum);
    REQUIRE(c->Read(c->Entries()[0], out, error));
    CHECK(out == data1);
}

TEST_CASE("kom: a kept header is written when it still describes re-encoded entries")
{
    // What a tool that extracts and re-encodes does (docs/tools/client-data.md): rebuild an archive of the other
    // packer's layout from decoded data at KOG's zlib level, then hand the original header back.
    const Bytes data1 = Pattern(5000, 11), data2 = Pattern(300, 12);
    const Bytes blob1 = Encoded("A.dds", kZlib, data1, kKogLevel), blob2 = Encoded("B.lua", kXorCrc, data2, kKogLevel);
    char xml[512];
    std::snprintf(xml, sizeof xml,
                  "<?xml version=\"1.0\" ?><Files>"
                  "<File Algorithm=\"0\" Checksum=\"%08x\" CompressedSize=\"%zu\" FileTime=\"00000000\" "
                  "Name=\"A.dds\" Size=\"5000\"/>"
                  "<File Algorithm=\"2\" Checksum=\"%08x\" CompressedSize=\"%zu\" FileTime=\"00000000\" "
                  "Name=\"B.lua\" Size=\"300\"/></Files>",
                  Adler32(blob1), blob1.size(), Adler32(blob2), blob2.size());
    const Bytes raw = RawArchive(xml, {blob1, blob2});
    const Bytes header(raw.begin(), raw.begin() + static_cast<std::ptrdiff_t>(kFixedHeaderSize + std::strlen(xml)));

    TempDir dir;
    std::string error;
    ArchiveBuilder b;
    b.SetLevel(kKogLevel);
    REQUIRE(b.Add("A.dds", kZlib, data1, error, 0));
    REQUIRE(b.Add("B.lua", kXorCrc, data2, error, 0));
    CHECK(b.Describes(header));
    b.SetHeader(header);
    REQUIRE(b.Write(dir / "same.kom", error));
    CHECK(ReadFile(dir / "same.kom") == raw);

    // Any difference in the entries and the header is generated instead.
    ArchiveBuilder level9;  // kom.py's level: other blobs, other checksums
    REQUIRE(level9.Add("A.dds", kZlib, data1, error, 0));
    REQUIRE(level9.Add("B.lua", kXorCrc, data2, error, 0));
    CHECK_FALSE(level9.Describes(header));
    ArchiveBuilder times = b;
    REQUIRE(times.Add("B.lua", kXorCrc, data2, error));  // FileTime = checksum
    CHECK_FALSE(times.Describes(header));
    ArchiveBuilder fewer;
    fewer.SetLevel(kKogLevel);
    REQUIRE(fewer.Add("A.dds", kZlib, data1, error, 0));
    CHECK_FALSE(fewer.Describes(header));
    fewer.SetHeader(header);
    REQUIRE(fewer.Write(dir / "fewer.kom", error));
    const auto f = OpenOrFail(dir / "fewer.kom");
    REQUIRE(f->Entries().size() == 1);
    const std::string fxml(f->RawHeader().begin() + kFixedHeaderSize, f->RawHeader().end());
    CHECK(fxml.starts_with("<?xml version=\"1.0\"?>"));

    Bytes corrupt = header;
    corrupt[60] ^= 1;  // file time
    CHECK_FALSE(b.Describes(corrupt));
    corrupt = header;
    corrupt.back() ^= 1;  // XML no longer matches its adler32
    CHECK_FALSE(b.Describes(corrupt));
    CHECK_FALSE(b.Describes(Bytes(header.begin(), header.end() - 1)));

    // The level is checked.
    Bytes blob;
    CHECK_FALSE(Encode("A.dds", kZlib, data1, blob, error, 10));
}

TEST_CASE("kom: rewrite with changes, in place")
{
    TempDir dir;
    const auto path = dir / "data001.kom";
    BuildSample(path);
    auto a = OpenOrFail(path);
    const Bytes before = ReadFile(path);

    ArchiveBuilder b = ArchiveBuilder::From(a);
    std::string error;
    REQUIRE(b.Add("texture_a.DDS", kZlib, ToBytes("replaced"), error));  // replaces in place, keeps the position
    CHECK(b.Remove("Later.lua"));
    CHECK_FALSE(b.Remove("Later.lua"));
    REQUIRE(b.Add("Added.lua", kXorCrc, Pattern(10, 9), error));
    CHECK(b.Count() == 5);
    REQUIRE_MESSAGE(b.Write(path, error), error);
    CHECK_FALSE(a->IsOpen());  // released before replacing its own file
    CHECK_FALSE(std::filesystem::exists(dir / "data001.kom.tmp"));

    const auto c = OpenOrFail(path);
    std::vector<std::string> names;
    for (const Entry& e : c->Entries())
        names.push_back(e.name);
    CHECK(names == std::vector<std::string>{"DLG_Sample.lua", "texture_a.DDS", "Empty.txt", "Old.bin", "Added.lua"});
    Bytes out;
    REQUIRE(c->Read(c->Entries()[0], out, error));
    CHECK(out == Pattern(3000, 1));
    REQUIRE(c->Read(c->Entries()[1], out, error));
    CHECK(out == ToBytes("replaced"));
    REQUIRE(c->ReadStored(c->Entries()[3], out, error));
    CHECK(out == Pattern(77, 4));
    CHECK(c->Entries()[3].fileTime == 0x12345678u);
    CHECK(ReadFile(path) != before);
}

TEST_CASE("kom: corrupted or foreign files are rejected")
{
    const Bytes blob = Encoded("a.lua", kZlib, Pattern(100, 1));
    char xml[256];
    std::snprintf(xml, sizeof xml,
                  "<?xml version=\"1.0\"?><Files><File Name=\"a.lua\" Size=\"100\" CompressedSize=\"%zu\" "
                  "Checksum=\"%08x\" FileTime=\"%08x\" Algorithm=\"0\" /></Files>",
                  blob.size(), Adler32(blob), Adler32(blob));
    const Bytes good = RawArchive(xml, {blob});
    {
        TempDir dir;
        WriteFile(dir / "good.kom", good);
        OpenOrFail(dir / "good.kom");
    }

    CHECK(OpenError(Bytes(10, 0)).find("too short") != std::string::npos);
    CHECK(OpenError(Bytes(200, 'x')).find("not a .kom") != std::string::npos);

    Bytes old = good;
    old[25] = '2';  // "V.0.2."
    CHECK(OpenError(old).find("unsupported .kom version V.0.2.") != std::string::npos);

    Bytes badAdler = good;
    badAdler[64] ^= 1;
    CHECK(OpenError(badAdler).find("XML header checksum") != std::string::npos);

    CHECK(OpenError(RawArchive(xml, {blob}, 0, 2)).find("says 2 entries") != std::string::npos);

    Bytes truncated = good;
    truncated.pop_back();
    CHECK(OpenError(truncated).find("past the end") != std::string::npos);

    CHECK(OpenError(RawArchive("<?xml version=\"1.0\"?><Files><File Name=\"a.lua\" Size=\"x\" /></Files>", {}, 0, 1))
              .find("bad Size") != std::string::npos);
    CHECK(OpenError(RawArchive("<Archive></Archive>", {})).find("not <Files>") != std::string::npos);
    CHECK(OpenError(RawArchive("<Files><File Name=\"a\" Size=\"1\" CompressedSize=\"0\"/>", {}, 0, 1))
              .find("bad XML header") != std::string::npos);
    CHECK(OpenError(RawArchive("<Files><File Name=\"a\"Size=\"1\" CompressedSize=\"0\"/></Files>", {}, 0, 1))
              .find("bad XML header") != std::string::npos);
    CHECK(OpenError(RawArchive("<Files></Files>junk", {}, 0, 0)).find("after </Files>") != std::string::npos);

    // An entry whose blob changed: the archive opens (only the header is read), the entry does not.
    Bytes flipped = good;
    flipped.back() ^= 0x40;
    TempDir dir;
    WriteFile(dir / "flipped.kom", flipped);
    const auto a = OpenOrFail(dir / "flipped.kom");
    Bytes out;
    std::string error;
    CHECK_FALSE(a->Read(a->Entries()[0], out, error));
    CHECK(error.find("checksum mismatch") != std::string::npos);

    // ... and a copy of it is refused too, leaving no partial file.
    CHECK_FALSE(ArchiveBuilder::From(a).Write(dir / "copy.kom", error));
    CHECK_FALSE(std::filesystem::exists(dir / "copy.kom"));
    CHECK_FALSE(std::filesystem::exists(dir / "copy.kom.tmp"));
}

TEST_CASE("kom: entity references and NUL padding in the XML header")
{
    const Bytes data = ToBytes("x");
    const Bytes blob = Encoded("a&b<c>\"d'.txt", kZlib, data);
    char xml[256];
    std::snprintf(xml, sizeof xml,
                  "<?xml version='1.0'?>\n<!-- c --><Files>\n  <File Name='a&amp;b&lt;c&gt;&quot;d&apos;.txt' Size='1' "
                  "CompressedSize='%zu' Checksum='%08X'></File>\n</Files>",
                  blob.size(), Adler32(blob));
    std::string padded = xml;
    padded.append(3, '\0');
    TempDir dir;
    WriteFile(dir / "e.kom", RawArchive(padded, {blob}));
    const auto a = OpenOrFail(dir / "e.kom");
    CHECK(a->Entries()[0].name == "a&b<c>\"d'.txt");
    CHECK(a->Entries()[0].algorithm == kZlib);  // default when absent

    // The generated header escapes it back.
    std::string error;
    ArchiveBuilder b = ArchiveBuilder::From(a);
    REQUIRE(b.Add("more.txt", kZlib, data, error));
    REQUIRE(b.Write(dir / "f.kom", error));
    const auto f = OpenOrFail(dir / "f.kom");
    CHECK(f->Entries()[0].name == "a&b<c>\"d'.txt");
    Bytes out;
    REQUIRE(f->Read(f->Entries()[0], out, error));
    CHECK(out == data);
}

TEST_CASE("kom: entry names are validated")
{
    ArchiveBuilder b;
    std::string error;
    CHECK_FALSE(b.Add("", kZlib, Bytes{}, error));
    CHECK_FALSE(b.Add("dir/file.lua", kZlib, Bytes{}, error));
    CHECK_FALSE(b.Add("dir\\file.lua", kZlib, Bytes{}, error));
    CHECK_FALSE(b.Add("tab\tname", kZlib, Bytes{}, error));
    CHECK_FALSE(b.Add("\xFF.lua", kZlib, Bytes{}, error));
    CHECK(b.Count() == 0);
}

TEST_CASE("kom: concurrent positional reads")
{
    TempDir dir;
    ArchiveBuilder b;
    std::string error;
    for (unsigned i = 0; i < 16; ++i)
        REQUIRE(b.Add("file" + std::to_string(i) + ".dat", i % 2 ? kXorCrc : kZlib, Pattern(5000 + i * 100, i),
                      error));
    REQUIRE(b.Write(dir / "c.kom", error));
    const auto a = OpenOrFail(dir / "c.kom");

    std::atomic<int> failures{0};
    std::vector<std::thread> threads;
    for (int t = 0; t < 8; ++t)
        threads.emplace_back([&, t] {
            Bytes out;
            std::string err;
            for (int round = 0; round < 40; ++round)
                for (unsigned i = 0; i < 16; ++i)
                {
                    const unsigned k = (i + static_cast<unsigned>(t)) % 16;
                    if (!a->Read(a->Entries()[k], out, err) || out != Pattern(5000 + k * 100, k))
                        ++failures;
                }
        });
    for (auto& th : threads)
        th.join();
    CHECK(failures == 0);
}

#if defined(_WIN32)
TEST_CASE("kom: replacing an archive another program has open fails clearly (Windows)")
{
    TempDir dir;
    BuildSample(dir / "data001.kom");
    const Bytes before = ReadFile(dir / "data001.kom");
    std::string error;
    // Opened as the original client opened its archives (FILE_SHARE_READ).
    auto held = x2::platform::ReadOnlyFile::Open(dir / "data001.kom", error);
    REQUIRE(held);
    ArchiveBuilder b;
    REQUIRE(b.Add("x.txt", kZlib, ToBytes("x"), error));
    CHECK_FALSE(b.Write(dir / "data001.kom", error));
    CHECK(error.find("running client") != std::string::npos);
    held.reset();
    CHECK(ReadFile(dir / "data001.kom") == before);
    CHECK_FALSE(std::filesystem::exists(dir / "data001.kom.tmp"));
}
#endif
