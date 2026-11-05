[Home](../README.md) › [Porting](README.md) › Client Lua data

# Client Lua data (`src/lua`, target `x2::lua`)

The client's scripts ship as **KOG LuaJIT 2.0.2 bytecode** inside the `.kom` archives, and its item tables as
`Item.kim`, a binary file KOG's `X2ScriptPreprocessingTool` builds from the item scripts. `x2::lua` turns both into
readable Lua and back, for [client_data](../tools/client-data.md#lua) and later for x2 studio. It is a C++ port of
`tools/kom/kom.py` (`decompile`) and `tools/kom/kim.py`, which stay its reference. It is separate from `x2::kom` (T35: Lua
belongs to the layer that manages that data) and runs scripts in the project's upstream LuaJIT 2.1 (T21).

| Header | Contents |
|---|---|
| `x2/lua/bytecode.h` | `IsBytecode` (`\x1bKL\x81`, `\x1bKL\x82`, stock `\x1bLJ\x01`), `ToLayout` (0x81 ↔ 0x82 opcode order), `kBom` |
| `x2/lua/decompile.h` | `Decompile`: stripped KOG bytecode → Lua source |
| `x2/lua/kim.h` | `LoadEnums` (Enum.lua), `DecompileKim` (Item.kim → Item.lua + SetItem.lua), `CompileKim` (back), `CheckSyntax` |

## Bytecode and decompiler

- KOG's dumps are stripped LuaJIT 2.0 dumps with the magic `\x1bKL`. Version `0x81` uses LuaJIT 2.0's opcode order;
  `0x82` permutes it (seven runs of opcodes, from `kom.py`'s `KlTranslate` table). The decompiler reads `0x81`; `ToLayout`
  converts between the two.
- `Decompile` is a **faithful port** of `kom.py`'s `FuncDecompiler`: same statement recovery, same names (`slotN`,
  `uvN`), same layout (indentation, 100-character table wrap counted in code points, Python's float `repr`). On the
  client's 4,553 bytecode scripts (2026-11-05) its output is byte for byte `kom.py`'s, with no failure; 9 more `.lua`
  entries are already source (`NPCTemplet.lua`, `Config_US_*.lua`, ...).
- Errors are returned, never thrown or crashed on: truncated dumps, unknown opcodes, debug info, big-endian dumps and
  what the decompiler cannot structure. Where Python would have crashed the port reports an error too, and a nesting
  guard (400 levels; real scripts stay under 100) bounds the recursion.
- Output is UTF-8 without a BOM, `\n` line ends; LuaJIT 2.1 loads all of the client's decompiled scripts.

## Item.kim

`Item.kim` (version `0x1201`: 188-byte item templets, interned strings and vectors, then the set records) is what the
client loads instead of running the item scripts. `DecompileKim` writes `Item.lua` and `SetItem.lua` in the layout of the
original scripts (UTF-8 with BOM, CRLF), in the order the tool read the items, so `CompileKim` of its output gives the
same `Item.kim` byte for byte. With the tables of `Enum.lua` (`LoadEnums`), enum fields are written by name
(`ITEM_TYPE["IT_WEAPON"]`); without them, as numbers.

`CompileKim` runs the scripts in LuaJIT behind a recording `g_pItemManager`/`g_pCX2SetItemManager`, then applies
`KProxy2::AddItemTemplet_LUA`'s defaults and checks (KOG's tool used Lua 5.1, whose unknown escapes such as `"\R"`
LuaJIT rejects: they are rewritten first). Scripts over 1 MB run in chunks of 1,000 statements. The warnings KOG's tool
reported (duplicated items, unknown set ids, enum names Enum.lua lacks) come back as warnings; the client's
`Item.kim` gives 24 (`INVALID_SETID_95`, ...) and round-trips: 49,837 items, 2,425 sets (2026-11-05).

## Tests (`ctest -R lua`)

`x2_lua_tests`, headless: headers, hand-assembled chunks (assignment, call, table) decompiled and accepted by LuaJIT, the
0x82 layout, every truncation of a dump and an unknown opcode reported as errors, `CheckSyntax`, and an Item.kim built
from scripts, decompiled with and without enum names and compiled back to the same bytes. The client's own scripts are
covered by the opt-in client_data test ([client_data › Tests](../tools/client-data.md#tests-ctest--r-assets)).
