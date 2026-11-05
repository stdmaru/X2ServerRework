// KOG's LuaJIT bytecode (docs/tools/client-data.md#lua): the client's .lua entries are stripped LuaJIT 2.0.2 dumps
// with KOG's header "\x1bKL\x81" (stock 2.0: "\x1bLJ\x01"). The later 2014 data files use "\x1bKL\x82": the same
// format with the opcodes reordered. tools/kom/kom.py is the reference.
#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace x2::lua {

// The UTF-8 BOM the client's script loader requires in front of Lua source (docs/porting/client-luacommon.md).
inline constexpr std::string_view kBom = "\xEF\xBB\xBF";

// "\x1bKL\x81", "\x1bKL\x82" or "\x1bLJ\x01".
bool IsBytecode(std::span<const std::uint8_t> data);

// A stripped KL dump in the opcode layout `version` (0x81 or 0x82). Anything that is not KL bytecode, or already
// in that layout, is copied as is.
bool ToLayout(std::span<const std::uint8_t> data, std::uint8_t version, std::vector<std::uint8_t>& out,
              std::string& error);

} // namespace x2::lua
