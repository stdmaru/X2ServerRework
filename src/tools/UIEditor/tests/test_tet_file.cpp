#include "tet_file.h"

#include <doctest/doctest.h>

#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

using x2::ui_editor::TetFile;
using x2::ui_editor::TetRect;

namespace {

TetRect Upright(float l, float t, float r, float b)
{
    TetRect rect;
    rect.leftTop[0] = l, rect.leftTop[1] = t;
    rect.rightBottom[0] = r, rect.rightBottom[1] = b;
    rect.leftBottom[0] = l, rect.leftBottom[1] = b;
    rect.rightTop[0] = r, rect.rightTop[1] = t;
    rect.size[0] = r - l, rect.size[1] = b - t;
    return rect;
}

// A .tet as the client's TETProxy::PostProcess writes it.
std::string Written(const std::vector<std::pair<std::u16string, TetRect>>& keys)
{
    std::string out;
    const auto u32 = [&](std::uint32_t v) { out.append(reinterpret_cast<const char*>(&v), 4); };
    u32(0x20544554);
    u32(0x100);
    u32(static_cast<std::uint32_t>(keys.size()));
    std::size_t offset = 12 + keys.size() * 44;
    for (const auto& key : keys) {
        u32(static_cast<std::uint32_t>(offset));
        offset += 2 + (key.first.size() + 1) * 2;
    }
    for (const auto& key : keys) {
        const TetRect& r = key.second;
        const float v[10] = {r.leftTop[0],    r.leftTop[1],  r.rightBottom[0], r.rightBottom[1], r.leftBottom[0],
                             r.leftBottom[1], r.rightTop[0], r.rightTop[1],    r.size[0],        r.size[1]};
        out.append(reinterpret_cast<const char*>(v), sizeof(v));
    }
    for (const auto& key : keys) {
        const auto length = static_cast<std::uint16_t>(key.first.size());
        out.append(reinterpret_cast<const char*>(&length), 2);
        out.append(reinterpret_cast<const char*>(key.first.data()), key.first.size() * 2);
        out.append(2, '\0');
    }
    return out;
}

}  // namespace

TEST_CASE("tet file: read and written back as the client writes it")
{
    const std::string bytes = Written({{u"BG_LEFT", Upright(0, 0, 31, 47)}, {u"BG_TOP", Upright(31, 0, 32, 47)}});
    TetFile file;
    std::string error;
    REQUIRE(TetFile::Parse(bytes, file, error));
    REQUIRE(file.Entries().size() == 2);
    CHECK(file.Entries()[0].key == "BG_LEFT");
    REQUIRE(file.Find("bg_top") != nullptr);
    CHECK(file.Find("bg_top")->leftTop[0] == 31);
    CHECK(file.Bytes() == bytes);

    CHECK_FALSE(TetFile::Parse(bytes.substr(0, 40), file, error));
    CHECK_FALSE(TetFile::Parse("TET?", file, error));
}

TEST_CASE("tet file: rectangles move and grow; keys are added in order")
{
    TetRect rect = Upright(10, 20, 30, 50);
    rect.Move(2, -3);
    CHECK(rect == Upright(12, 17, 32, 47));
    REQUIRE(rect.Resize(5, 1));
    CHECK(rect == Upright(12, 17, 37, 48));

    TetRect rotated = Upright(0, 0, 10, 20);
    std::swap(rotated.leftTop, rotated.rightBottom);
    CHECK_FALSE(rotated.Upright());
    CHECK_FALSE(rotated.Resize(1, 1));

    TetFile file;
    std::string error;
    REQUIRE(TetFile::Parse(Written({{u"A", Upright(0, 0, 1, 1)}, {u"C", Upright(0, 0, 2, 2)}}), file, error));
    REQUIRE(file.Add("b_new", Upright(0, 0, 3, 3), error));
    CHECK_FALSE(file.Add("B_NEW", Upright(0, 0, 3, 3), error));
    CHECK_FALSE(file.Add("bad key", Upright(0, 0, 3, 3), error));
    REQUIRE(file.Entries().size() == 3);
    CHECK(file.Entries()[1].key == "B_NEW");
    CHECK(file.Set("c", Upright(1, 1, 2, 2)));
    CHECK_FALSE(file.Set("D", Upright(1, 1, 2, 2)));
    CHECK(file.Bytes() == Written({{u"A", Upright(0, 0, 1, 1)}, {u"B_NEW", Upright(0, 0, 3, 3)}, {u"C", Upright(1, 1, 2, 2)}}));

    CHECK(x2::ui_editor::TetName("DLG_UI_Common_Texture49_NEW.TGA") == "DLG_UI_Common_Texture49_NEW.TET");
}

TEST_CASE("tet file: keys are removed and renamed; a file with no keys is a .tet")
{
    TetFile file;
    std::string error;
    REQUIRE(TetFile::Parse(Written({{u"A", Upright(0, 0, 1, 1)}, {u"C", Upright(0, 0, 2, 2)}}), file, error));
    CHECK_FALSE(file.Remove("B"));
    REQUIRE(file.Rename("a", "z_key", error));
    CHECK(file.Entries()[0].key == "C");
    CHECK(file.Entries()[1].key == "Z_KEY");
    CHECK(file.Find("z_key")->size[0] == 1);
    CHECK_FALSE(file.Rename("C", "z_key", error));      // taken
    CHECK_FALSE(file.Rename("C", "no good", error));
    CHECK_FALSE(file.Rename("nothing", "x", error));
    CHECK(file.Rename("c", "C", error));                // the same name: nothing to do
    REQUIRE(file.Remove("c"));
    REQUIRE(file.Remove("Z_KEY"));
    CHECK(file.Entries().empty());

    // The client takes a file with no keys (OpenFile), and it reads back.
    TetFile again;
    REQUIRE_MESSAGE(TetFile::Parse(file.Bytes(), again, error), error);
    CHECK(again.Entries().empty());
    CHECK(again.Bytes() == file.Bytes());
}

TEST_CASE("tet file: the scripts that name a texture's key")
{
    const std::filesystem::path game = std::filesystem::temp_directory_path() / "x2_ui_editor_tet_uses";
    std::filesystem::remove_all(game);
    std::filesystem::create_directories(game / "data" / "ui" / "scripts");
    std::filesystem::create_directories(game / "data" / ".kom");
    std::ofstream(game / "data" / "ui" / "scripts" / "DLG_A.lua")
        << "p:SetTex(\"tex.tga\", \"KEY\")\np:SetTex(\"tex.tga\", \"KEY_2\")\n\np:SetTex('TEX.TGA', 'key')\n";
    std::ofstream(game / "data" / "ui" / "scripts" / "DLG_B.lua") << "p:SetTex(\"other.tga\", \"KEY\")\n";
    std::ofstream(game / "data" / ".kom" / "DLG_C.lua") << "p:SetTex(\"tex.tga\", \"KEY\")\n";
    const auto uses = x2::ui_editor::FindKeyUses(game, "TEX.tga", "key");
    REQUIRE(uses.size() == 2);
    CHECK(uses[0].script == std::filesystem::path("ui/scripts/DLG_A.lua"));
    CHECK(uses[0].line == 1);
    CHECK(uses[1].line == 4);
    std::filesystem::remove_all(game);
}

// Opt-in, on a real client_data: every .tet reads and writes back the same bytes.
TEST_CASE("tet file: the game's .tet files write back unchanged")
{
    const char* game = std::getenv("X2_UI_EDITOR_GAME");
    if (game == nullptr) {
        return;
    }
    int files = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::path(game) / "data")) {
        std::string ext = entry.path().extension().string();
        for (char& c : ext) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        if (!entry.is_regular_file() || ext != ".tet") {
            continue;
        }
        std::ifstream in(entry.path(), std::ios::binary);
        const std::string bytes(std::istreambuf_iterator<char>(in), {});
        TetFile file;
        std::string error;
        INFO(entry.path().string());
        REQUIRE(TetFile::Parse(bytes, file, error));
        CHECK(file.Bytes() == bytes);
        ++files;
    }
    MESSAGE(files << " .tet files");
}
