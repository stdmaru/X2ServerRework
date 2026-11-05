#include "x2/lua/bytecode.h"
#include "x2/lua/decompile.h"

#include <doctest/doctest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace {

std::vector<std::uint8_t> ReadFile(const std::filesystem::path& p)
{
    std::ifstream in(p, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

} // namespace

TEST_CASE("decompile: scratch oracle comparison")
{
    const char* bc = std::getenv("X2_LUA_ORACLE_BC");
    const char* py = std::getenv("X2_LUA_ORACLE_PY");
    if (!bc || !py)
        return;
    int same = 0, differ = 0, failed = 0;
    for (const auto& e : std::filesystem::directory_iterator(py))
    {
        const auto data = ReadFile(std::filesystem::path(bc) / e.path().filename());
        const auto expected = ReadFile(e.path());
        std::string source, error;
        if (!x2::lua::Decompile(data, source, error))
        {
            ++failed;
            MESSAGE(e.path().filename().string() << ": " << error);
            continue;
        }
        const std::string want(expected.begin() + 3, expected.end());
        if (source == want)
            ++same;
        else
        {
            ++differ;
            if (differ <= 5)
            {
                std::ofstream(e.path().filename().string() + ".cpp.lua", std::ios::binary) << source;
                MESSAGE("differs: " << e.path().filename().string());
            }
        }
    }
    MESSAGE("same " << same << " differ " << differ << " failed " << failed);
    CHECK(differ == 0);
    CHECK(failed == 0);
}

#include "x2/lua/kim.h"

TEST_CASE("kim: scratch oracle comparison")
{
    const char* dir = std::getenv("X2_LUA_ORACLE_KIM");
    const char* py = std::getenv("X2_LUA_ORACLE_PY");
    if (!dir || !py)
        return;
    const std::filesystem::path d(dir);
    const auto kim = ReadFile(d / "Item.kim");
    const auto enumBytes = ReadFile(std::filesystem::path(py) / "Enum.lua");
    const std::vector<std::string> enumSrc{std::string(enumBytes.begin(), enumBytes.end())};
    x2::lua::Enums enums;
    std::string error;
    REQUIRE_MESSAGE(x2::lua::LoadEnums(enumSrc, enums, error), error);
    x2::lua::ItemScripts scripts;
    REQUIRE_MESSAGE(x2::lua::DecompileKim(kim, enums, scripts, error), error);
    const auto body = [](const std::string& s) { return s.substr(s.find("\r\n\r\n\r\n")); };
    const auto wantItems = ReadFile(d / "enum" / "Item.lua");
    const auto wantSets = ReadFile(d / "enum" / "SetItem.lua");
    const bool itemsSame = body(scripts.items) == body(std::string(wantItems.begin(), wantItems.end()));
    const bool setsSame = body(scripts.setItems) == body(std::string(wantSets.begin(), wantSets.end()));
    if (!itemsSame)
        std::ofstream("Item.cpp.lua", std::ios::binary) << scripts.items;
    if (!setsSame)
        std::ofstream("SetItem.cpp.lua", std::ios::binary) << scripts.setItems;
    CHECK(itemsSame);
    CHECK(setsSame);
    std::vector<std::uint8_t> rebuilt;
    std::vector<std::string> warnings;
    REQUIRE_MESSAGE(x2::lua::CompileKim({scripts.items, scripts.setItems}, enumSrc, rebuilt, warnings, error), error);
    MESSAGE(warnings.size() << " warnings; " << rebuilt.size() << " bytes");
    if (rebuilt != kim)
        std::ofstream("rebuilt.kim", std::ios::binary).write(reinterpret_cast<const char*>(rebuilt.data()),
                                                            static_cast<std::streamsize>(rebuilt.size()));
    CHECK(rebuilt == kim);
}
