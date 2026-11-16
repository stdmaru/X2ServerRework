#include "helpers.h"

#include "x2/assets/texture_sizes.h"

using namespace x2::assets;
using namespace assets_test;

namespace {

Bytes Tga(std::uint16_t width, std::uint16_t height)
{
    Bytes tga(18 + std::size_t{width} * height * 4, 0);
    tga[2] = 2;
    tga[12] = static_cast<std::uint8_t>(width);
    tga[13] = static_cast<std::uint8_t>(width >> 8);
    tga[14] = static_cast<std::uint8_t>(height);
    tga[15] = static_cast<std::uint8_t>(height >> 8);
    tga[16] = 32;
    return tga;
}

Bytes Dds(std::uint32_t width, std::uint32_t height)
{
    Bytes dds(128 + 16, 0);
    std::memcpy(dds.data(), "DDS ", 4);
    PutU32(dds, 12, height);
    PutU32(dds, 16, width);
    return dds;
}

} // namespace

TEST_CASE("assets: image sizes from their headers")
{
    std::uint32_t w = 0, h = 0;
    Bytes tga = Tga(300, 2);
    REQUIRE(ImageSize(tga.data(), tga.size(), w, h));
    CHECK((w == 300 && h == 2));
    Bytes dds = Dds(512, 128);
    REQUIRE(ImageSize(dds.data(), dds.size(), w, h));
    CHECK((w == 512 && h == 128));
    Bytes bmp(54, 0);
    bmp[0] = 'B';
    bmp[1] = 'M';
    PutU32(bmp, 18, 64);
    PutU32(bmp, 22, static_cast<std::uint32_t>(-32));  // top-down
    REQUIRE(ImageSize(bmp.data(), bmp.size(), w, h));
    CHECK((w == 64 && h == 32));
    Bytes png = ToBytes(std::string_view("\x89PNG\r\n\x1a\n\0\0\0\x0dIHDR\0\0\x01\0\0\0\0\x20", 24));
    REQUIRE(ImageSize(png.data(), png.size(), w, h));
    CHECK((w == 256 && h == 32));
    // SOI, an APP0 segment, then SOF0: precision, height 48, width 80.
    Bytes jpg = {0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x04, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x00, 0x30, 0x00, 0x50};
    REQUIRE(ImageSize(jpg.data(), jpg.size(), w, h));
    CHECK((w == 80 && h == 48));
    Bytes text = ToBytes("not an image at all, long enough");
    CHECK_FALSE(ImageSize(text.data(), text.size(), w, h));
}

TEST_CASE("assets: texture-sizes lists the first entry of every image name, sorted")
{
    TempDir dir;
    std::filesystem::create_directories(dir / "data");
    std::string error;
    {
        x2::kom::ArchiveBuilder a;
        REQUIRE_MESSAGE(a.Add("b.dds", 2, Dds(64, 32), error), error);
        REQUIRE_MESSAGE(a.Add("A.tga", 2, Tga(16, 8), error), error);
        REQUIRE_MESSAGE(a.Add("notes.txt", 2, ToBytes("text"), error), error);
        REQUIRE_MESSAGE(a.Add("empty.tga", 2, ToBytes(""), error), error);
        REQUIRE_MESSAGE(a.Write(dir / "data" / "data001.kom", error), error);
        x2::kom::ArchiveBuilder b;
        REQUIRE_MESSAGE(b.Add("B.DDS", 2, Dds(1, 1), error), error);  // later archive: the first one wins
        REQUIRE_MESSAGE(b.Write(dir / "data" / "data002.kom", error), error);
    }
    TextureSizeStats stats;
    REQUIRE_MESSAGE(WriteTextureSizes(dir / "data", dir / "sizes.txt", 2, stats, error), error);
    CHECK(stats.images == 2);
    CHECK(stats.skipped == 1);
    const Bytes out = ReadFile(dir / "sizes.txt");
    const std::string text(out.begin(), out.end());
    CHECK(text.substr(text.find("\nA.tga")) == "\nA.tga\t16\t8\nb.dds\t64\t32\n");
}
