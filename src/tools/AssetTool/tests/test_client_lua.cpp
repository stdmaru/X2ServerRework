// Import and pack of the Lua forms (docs/tools/client-data.md#lua): bytecode becomes source, Item.kim becomes
// Item.lua + SetItem.lua, unchanged files pack back from the originals and edited ones as source.
#include "helpers.h"

#include "x2/assets/client_data.h"
#include "x2/lua/bytecode.h"
#include "x2/lua/decompile.h"
#include "x2/lua/kim.h"

using namespace x2::assets;
using namespace assets_test;
namespace fs = std::filesystem;

namespace {

// `x = 42`, as KOG's tool dumps it (stripped, layout 0x81).
const Bytes kScript = {0x1B, 'K', 'L', 0x81, 0x02, 0x15, 0x02, 0x00, 0x01, 0x00, 0x01, 0x00, 0x03,
                       0x27, 0x00, 0x2A, 0x00, 0x35, 0x00, 0x00, 0x00, 0x47, 0x00, 0x01, 0x00, 0x06, 'x', 0x00};

const std::string kBom(x2::lua::kBom);
const std::string kEnum = kBom + "ITEM_TYPE = { IT_NONE = 0, IT_WEAPON = 1 }\n";
const std::string kItems = kBom + "g_pItemManager:AddItemTemplet\n{\n    m_ItemID = 100,\n    m_Name = \"Sword\",\n"
                                  "    m_ItemType = ITEM_TYPE[\"IT_WEAPON\"],\n    m_SetID = 7,\n}\n";
const std::string kSets = kBom + "g_pCX2SetItemManager:AddSetItemData_LUA\n{\n    m_SetID = 7,\n"
                                 "    m_SetName = \"Starter\",\n    m_NeedPartsNum = 1,\n}\n";

std::string Text(const Bytes& b) { return std::string(b.begin(), b.end()); }

struct LuaFixture
{
    TempDir dir;
    fs::path source = dir / "data";
    fs::path clientData = dir / "client_data";
    fs::path out = dir / "out";
    fs::path unsorted = clientData / "_unsorted" / "a";
    Bytes kim;

    LuaFixture()
    {
        std::vector<std::string> warnings;
        std::string error;
        REQUIRE_MESSAGE(x2::lua::CompileKim({kItems, kSets}, {kEnum}, kim, warnings, error), error);
        x2::kom::ArchiveBuilder a;
        a.SetLevel(x2::kom::kKogLevel);
        REQUIRE(a.Add("Script.lua", x2::kom::kXorCrc, kScript, error));
        REQUIRE(a.Add("Broken.lua", x2::kom::kXorCrc, Bytes(kScript.begin(), kScript.begin() + 12), error));
        REQUIRE(a.Add("Enum.lua", x2::kom::kXorCrc, ToBytes(kEnum), error));  // already source, as some are
        REQUIRE(a.Add("Item.kim", x2::kom::kZlib, kim, error));
        fs::create_directories(source);
        REQUIRE_MESSAGE(a.Write(source / "a.kom", error), error);
    }

    ImportStats DoImport(bool readableLua = true)
    {
        ImportOptions options;
        options.source = source;
        options.destination = clientData;
        options.readableLua = readableLua;
        ImportStats stats;
        std::string error;
        REQUIRE_MESSAGE(Import(options, stats, error), error);
        return stats;
    }

    bool DoPack(PackStats& stats, std::string& error)
    {
        PackOptions options;
        options.clientData = clientData;
        options.destination = out;
        return Pack(options, stats, error);
    }

    Status DoStatus()
    {
        Status status;
        std::string error;
        REQUIRE_MESSAGE(GetStatus(clientData, 0, status, error), error);
        return status;
    }

    Bytes Entry(const std::string& name)
    {
        std::string error;
        auto archive = x2::kom::Archive::Open(out / "a.kom", error);
        REQUIRE_MESSAGE(archive, error);
        const x2::kom::Entry* e = archive->Find(name);
        REQUIRE_MESSAGE(e, name);
        Bytes data;
        REQUIRE_MESSAGE(archive->Read(*e, data, error), error);
        return data;
    }
};

} // namespace

TEST_CASE("assets: import makes the Lua readable and packs the originals back")
{
    LuaFixture f;
    const ImportStats stats = f.DoImport();
    CHECK(stats.files == 4);
    CHECK(stats.luaSource == 1);
    CHECK(stats.luaKept == 1);
    CHECK(stats.kim == 1);

    CHECK(Text(ReadFile(f.unsorted / "Script.lua")) == kBom + "x = 42\n");
    CHECK(x2::lua::IsBytecode(ReadFile(f.unsorted / "Broken.lua")));
    CHECK(Text(ReadFile(f.unsorted / "Enum.lua")) == kEnum);
    CHECK_FALSE(fs::exists(f.unsorted / "Item.kim"));
    const std::string items = Text(ReadFile(f.unsorted / "Item.lua"));
    CHECK(items.find("ITEM_TYPE[\"IT_WEAPON\"]") != std::string::npos);  // with the names of Enum.lua
    CHECK(Text(ReadFile(f.unsorted / "SetItem.lua")).find("Starter") != std::string::npos);
    CHECK(ReadFile(f.clientData / ".kom" / "originals" / "Script.lua") == kScript);
    CHECK(ReadFile(f.clientData / ".kom" / "originals" / "Item.kim") == f.kim);
    CHECK_FALSE(fs::exists(f.clientData / ".kom" / "originals" / "Broken.lua"));

    const Status status = f.DoStatus();
    CHECK(status.changed.empty());
    CHECK(status.missing.empty());
    CHECK(status.added.empty());

    PackStats packed;
    std::string error;
    REQUIRE_MESSAGE(f.DoPack(packed, error), error);
    CHECK(packed.identicalHeaders == 1);
    CHECK(packed.luaSource == 0);
    CHECK(packed.kimCompiled == 0);
    CHECK(ReadFile(f.out / "a.kom") == ReadFile(f.source / "a.kom"));
}

TEST_CASE("assets: edited scripts pack as source and Item.kim is compiled again")
{
    LuaFixture f;
    f.DoImport();
    WriteFile(f.unsorted / "Script.lua", ToBytes("x = 43\n"));  // no BOM: pack adds it
    std::string sets = Text(ReadFile(f.unsorted / "SetItem.lua"));
    sets.replace(sets.find("Starter"), 7, "Beginner");
    WriteFile(f.unsorted / "SetItem.lua", ToBytes(sets));

    const Status status = f.DoStatus();
    CHECK(status.changed == std::vector<std::string>{"Script.lua", "Item.kim"});
    CHECK(status.added.empty());

    PackStats packed;
    std::string error;
    REQUIRE_MESSAGE(f.DoPack(packed, error), error);
    CHECK(packed.changed == 2);
    CHECK(packed.luaSource == 1);
    CHECK(packed.kimCompiled == 1);
    CHECK(Text(f.Entry("Script.lua")) == kBom + "x = 43\n");
    CHECK(f.Entry("Broken.lua") == Bytes(kScript.begin(), kScript.begin() + 12));
    x2::lua::Enums enums;
    x2::lua::ItemScripts scripts;
    REQUIRE_MESSAGE(x2::lua::DecompileKim(f.Entry("Item.kim"), enums, scripts, error), error);
    CHECK(scripts.setItems.find("Beginner") != std::string::npos);
    CHECK(scripts.itemCount == 1);

    // A script LuaJIT cannot load stops the pack, naming it.
    WriteFile(f.unsorted / "Script.lua", ToBytes("x = = 1\n"));
    CHECK_FALSE(f.DoPack(packed, error));
    CHECK_MESSAGE(error.find("Script.lua") != std::string::npos, error);
    WriteFile(f.unsorted / "Script.lua", ToBytes("x = 43\n"));

    // So do item scripts the item tool would reject.
    WriteFile(f.unsorted / "Item.lua", ToBytes("g_pItemManager:AddItemTemplet{ m_ItemID = "));
    CHECK_FALSE(f.DoPack(packed, error));
    CHECK_MESSAGE(error.find("Item.kim") != std::string::npos, error);

    // Without either script, Item.kim is missing.
    fs::remove(f.unsorted / "SetItem.lua");
    CHECK(f.DoStatus().missing == std::vector<std::string>{"Item.kim"});
}

TEST_CASE("assets: import without readable Lua keeps the scripts as they are")
{
    LuaFixture f;
    const ImportStats stats = f.DoImport(false);
    CHECK(stats.luaSource == 0);
    CHECK(stats.kim == 0);
    CHECK(ReadFile(f.unsorted / "Script.lua") == kScript);
    CHECK(ReadFile(f.unsorted / "Item.kim") == f.kim);
    CHECK_FALSE(fs::exists(f.unsorted / "Item.lua"));
    PackStats packed;
    std::string error;
    REQUIRE_MESSAGE(f.DoPack(packed, error), error);
    CHECK(ReadFile(f.out / "a.kom") == ReadFile(f.source / "a.kom"));
}
