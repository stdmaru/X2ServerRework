// Headless tests of x2::lua: hand-assembled KOG bytecode, the 0x81/0x82 layouts and an Item.kim round trip. The
// client's own scripts are covered by the opt-in client_data test (src/tools/AssetTool/tests/test_client_import.cpp).
#include "x2/lua/bytecode.h"
#include "x2/lua/decompile.h"
#include "x2/lua/kim.h"

#include <doctest/doctest.h>

#include <cstdint>
#include <string>
#include <vector>

using namespace x2::lua;

namespace {

using Bytes = std::vector<std::uint8_t>;

// Stock LuaJIT 2.0 opcodes (the 0x81 layout).
enum Op : std::uint8_t
{
    KSTR = 37,
    KSHORT = 39,
    GGET = 52,
    GSET = 53,
    TNEW = 50,
    TSETS = 58,
    CALL = 62,
    RET0 = 71,
};

std::uint32_t AD(Op op, std::uint32_t a, std::uint32_t d) { return op | (a << 8) | (d << 16); }
std::uint32_t ABC(Op op, std::uint32_t a, std::uint32_t b, std::uint32_t c)
{
    return op | (a << 8) | (c << 16) | (b << 24);
}

void Uleb(Bytes& out, std::uint32_t v)
{
    do
    {
        std::uint8_t b = v & 0x7F;
        v >>= 7;
        out.push_back(static_cast<std::uint8_t>(b | (v ? 0x80 : 0)));
    } while (v);
}

// A stripped dump of one vararg main chunk. Instructions refer to strings[i] as i; the dump lists them backwards.
Bytes Chunk(const std::vector<std::uint32_t>& code, const std::vector<std::string>& strings, int framesize,
            std::uint8_t version = 0x81)
{
    Bytes proto = {0x02 /* vararg */, 0, static_cast<std::uint8_t>(framesize), 0};
    Uleb(proto, static_cast<std::uint32_t>(strings.size()));
    Uleb(proto, 0);
    Uleb(proto, static_cast<std::uint32_t>(code.size()));
    for (const std::uint32_t ins : code)
        for (int i = 0; i < 4; ++i)
            proto.push_back(static_cast<std::uint8_t>(ins >> (8 * i)));
    for (auto it = strings.rbegin(); it != strings.rend(); ++it)
    {
        Uleb(proto, static_cast<std::uint32_t>(5 + it->size()));
        proto.insert(proto.end(), it->begin(), it->end());
    }
    Bytes out = {0x1B, 'K', 'L', version};
    Uleb(out, 0x02);  // stripped
    Uleb(out, static_cast<std::uint32_t>(proto.size()));
    out.insert(out.end(), proto.begin(), proto.end());
    out.push_back(0);
    return out;
}

std::string Decompiled(const Bytes& bytecode)
{
    std::string source, error;
    REQUIRE_MESSAGE(Decompile(bytecode, source, error), error);
    return source;
}

} // namespace

TEST_CASE("lua: bytecode headers")
{
    CHECK(IsBytecode(Bytes{0x1B, 'K', 'L', 0x81}));
    CHECK(IsBytecode(Bytes{0x1B, 'K', 'L', 0x82}));
    CHECK(IsBytecode(Bytes{0x1B, 'L', 'J', 0x01}));
    CHECK_FALSE(IsBytecode(Bytes{0x1B, 'L', 'J', 0x02}));  // LuaJIT 2.1
    CHECK_FALSE(IsBytecode(Bytes{0x1B, 'K', 'L'}));
    const std::string source = std::string(kBom) + "x = 1";
    CHECK_FALSE(IsBytecode(Bytes(source.begin(), source.end())));
}

TEST_CASE("lua: decompiles hand-assembled chunks")
{
    // x = 42
    CHECK(Decompiled(Chunk({AD(KSHORT, 0, 42), AD(GSET, 0, 0), AD(RET0, 0, 1)}, {"x"}, 1)) == "x = 42\n");
    // print("hi")
    const Bytes call = Chunk({AD(GGET, 0, 0), AD(KSTR, 1, 1), ABC(CALL, 0, 1, 2), AD(RET0, 0, 1)}, {"print", "hi"}, 2);
    const std::string printed = Decompiled(call);
    CHECK(printed == "print(\"hi\")\n");
    // t = {}; t.name = "a"
    const std::string table = Decompiled(Chunk({AD(TNEW, 0, 0), AD(KSTR, 1, 1), ABC(TSETS, 1, 0, 2),
                                                AD(GSET, 0, 0), AD(RET0, 0, 1)},
                                               {"t", "a", "name"}, 2));
    CHECK(table.find("name = \"a\"") != std::string::npos);
    std::string error;
    CHECK_MESSAGE(CheckSyntax(printed, "print", error), error);
    CHECK_MESSAGE(CheckSyntax(table, "table", error), error);
}

TEST_CASE("lua: the 0x82 layout reorders the opcodes")
{
    const Bytes stock = Chunk({AD(GGET, 0, 0), AD(KSTR, 1, 1), ABC(CALL, 0, 1, 2), AD(RET0, 0, 1)}, {"print", "hi"}, 2);
    Bytes kog, back;
    std::string error;
    REQUIRE_MESSAGE(ToLayout(stock, 0x82, kog, error), error);
    CHECK(kog[3] == 0x82);
    CHECK(kog != stock);
    CHECK(Decompiled(kog) == Decompiled(stock));
    REQUIRE_MESSAGE(ToLayout(kog, 0x81, back, error), error);
    CHECK(back == stock);
}

TEST_CASE("lua: bad bytecode is an error, not a crash")
{
    const Bytes good = Chunk({AD(KSHORT, 0, 42), AD(GSET, 0, 0), AD(RET0, 0, 1)}, {"x"}, 1);
    std::string source, error;
    for (std::size_t n = 0; n < good.size(); ++n)
    {
        CAPTURE(n);
        CHECK_FALSE(Decompile(std::span(good).first(n), source, error));
    }
    Bytes unknown = good;
    unknown[unknown.size() - 15] = 0xF0;  // an opcode past FUNCCW
    CHECK_FALSE(Decompile(unknown, source, error));
    CHECK_FALSE(Decompile(Bytes{'x', ' ', '=', ' ', '1'}, source, error));
}

TEST_CASE("lua: syntax check")
{
    std::string error;
    CHECK(CheckSyntax(std::string(kBom) + "local a = 1\nreturn a\n", "ok", error));
    CHECK_FALSE(CheckSyntax("local = 1", "bad", error));
    CHECK(error.find("bad") != std::string::npos);
}

namespace {

const std::string kEnum = std::string(kBom) + R"(ITEM_TYPE = { IT_NONE = 0, IT_WEAPON = 1, IT_DEFENCE = 2 }
)";

const std::string kItems = std::string(kBom) + R"(g_pItemManager:AddItemTemplet
{
    m_ItemID = 100,
    m_Name = "Wooden Sword",
    m_ItemType = ITEM_TYPE["IT_WEAPON"],
    m_Description = "S\R",
    m_Stat = { m_fBaseHP = 10, m_fAtkPhysic = 2.5 },
    SOCKET_OPTION = { 3, 4 },
}
g_pItemManager:AddItemTemplet
{
    m_ItemID = 101,
    m_Name = "Cloth",
    m_ItemType = ITEM_TYPE["IT_DEFENCE"],
    m_SetID = 7,
}
)";

const std::string kSets = std::string(kBom) + R"(g_pCX2SetItemManager:AddSetItemData_LUA
{
    m_SetID = 7,
    m_SetName = "Starter",
    m_NeedPartsNum = 1,
    m_Option1 = 12,
}
)";

} // namespace

TEST_CASE("lua: Item.kim round trip")
{
    Bytes kim;
    std::vector<std::string> warnings;
    std::string error;
    REQUIRE_MESSAGE(CompileKim({kItems, kSets}, {kEnum}, kim, warnings, error), error);
    CHECK(warnings.empty());

    Enums enums;
    REQUIRE_MESSAGE(LoadEnums({kEnum}, enums, error), error);
    CHECK(enums.tables.at("ITEM_TYPE").at("IT_WEAPON") == 1.0);

    ItemScripts scripts;
    REQUIRE_MESSAGE(DecompileKim(kim, enums, scripts, error), error);
    CHECK(scripts.itemCount == 2);
    CHECK(scripts.setCount == 1);
    CHECK(scripts.items.starts_with(kBom));
    CHECK(scripts.items.find("ITEM_TYPE[\"IT_WEAPON\"]") != std::string::npos);
    CHECK(scripts.items.find("Wooden Sword") != std::string::npos);
    CHECK(scripts.setItems.find("Starter") != std::string::npos);

    // The decompiled scripts build the same Item.kim, with or without enum names.
    Bytes again;
    REQUIRE_MESSAGE(CompileKim({scripts.items, scripts.setItems}, {kEnum}, again, warnings, error), error);
    CHECK(again == kim);
    ItemScripts plain;
    REQUIRE_MESSAGE(DecompileKim(kim, Enums{}, plain, error), error);
    CHECK(plain.items.find("ITEM_TYPE") == std::string::npos);
    REQUIRE_MESSAGE(CompileKim({plain.items, plain.setItems}, {}, again, warnings, error), error);
    CHECK(again == kim);

    // Lua errors are reported, not thrown.
    CHECK_FALSE(CompileKim({"g_pItemManager:AddItemTemplet{ m_ItemID = "}, {}, again, warnings, error));
    CHECK_FALSE(error.empty());
    CHECK_FALSE(DecompileKim(Bytes{1, 2, 3}, enums, scripts, error));
    kim.resize(kim.size() / 2);
    CHECK_FALSE(DecompileKim(kim, enums, scripts, error));
}
