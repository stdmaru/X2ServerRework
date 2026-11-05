// Decompiler for KOG's LuaJIT 2.0 bytecode (docs/tools/client-data.md#lua). A port of tools/kom/kom.py's
// decompiler, which stays its reference: for the same bytecode both write the same text, byte for byte. That text
// was validated on every script of the client: recompiling it with the client's LuaJIT gives equivalent bytecode.
#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace x2::lua {

// Decompiles a stripped dump (any layout IsBytecode accepts) into Lua source: UTF-8, without the BOM, "\n" line
// ends. Fails on what the decompiler does not handle (debug info, cdata constants, irreducible control flow, ...).
bool Decompile(std::span<const std::uint8_t> bytecode, std::string& source, std::string& error);

} // namespace x2::lua
