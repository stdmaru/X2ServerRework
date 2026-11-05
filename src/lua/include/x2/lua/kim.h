// Item.kim <-> Item.lua + SetItem.lua (docs/tools/client-data.md#itemkim). Item.kim is what KOG's
// X2ScriptPreprocessingTool builds from the item scripts (X2ItemManager_Preprocessing.cpp, KProxy2); the client loads
// it instead of running them. A port of tools/kom/kim.py, which stays its reference: Item.kim -> scripts -> Item.kim
// reproduces the file byte for byte (the scripts list the items in the order the tool originally read them).
#pragma once

#include <cstdint>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace x2::lua {

// Enum names for the item fields (ITEM_TYPE, UNIT_CLASS, ...), read from the client's Enum.lua.
struct Enums
{
    // table -> name -> value
    std::map<std::string, std::map<std::string, double>> tables;

    bool Empty() const { return tables.empty(); }
};

// Runs the Enum.lua sources (UTF-8 Lua source, BOM optional) with LuaJIT and collects the tables Item.lua uses.
bool LoadEnums(const std::vector<std::string>& enumSources, Enums& enums, std::string& error);

struct ItemScripts
{
    std::string items;     // Item.lua: UTF-8 with BOM, CRLF line ends
    std::string setItems;  // SetItem.lua
    std::size_t itemCount = 0, setCount = 0;
};

// Item.kim -> Item.lua + SetItem.lua; with enums, enum fields are written by name (ITEM_TYPE["IT_WEAPON"]).
bool DecompileKim(std::span<const std::uint8_t> kim, const Enums& enums, ItemScripts& scripts, std::string& error);

// The item scripts (in the order the tool reads them: Item.lua, then SetItem.lua) -> Item.kim. The scripts run in
// LuaJIT after the Enum.lua sources. `warnings` gets what KOG's tool also reports (duplicated items, unknown set
// ids, enum names Enum.lua does not define).
bool CompileKim(const std::vector<std::string>& scripts, const std::vector<std::string>& enumSources,
                std::vector<std::uint8_t>& kim, std::vector<std::string>& warnings, std::string& error);

// Does LuaJIT accept the source (UTF-8, BOM optional)? `error` gets its message.
bool CheckSyntax(std::string_view source, std::string_view chunkName, std::string& error);

} // namespace x2::lua
