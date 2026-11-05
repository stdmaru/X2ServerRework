// Entry encodings of .kom archives (decision T35, docs/porting/kom.md). Reading side: what the client needs.
#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace x2::kom {

// The XML header's Algorithm attribute.
enum Algorithm : std::uint32_t
{
    kZlib = 0,      // blob = zlib(data)
    kBlowfish = 1,  // zlib + Blowfish; no client of ours uses it, entries are kept opaque (copied, never decoded)
    kXorCrc = 2,    // blob = XORCRC(zlib(data)), the 2014 client's encrypted Lua
    kXorCrc3 = 3,   // blob = XORCRC3(prefix[4] + zlib(data) without its adler32 trailer), later (2014-08+) data files
};

bool IsDecodable(std::uint32_t algorithm);

std::uint32_t Adler32(std::span<const std::uint8_t> data);

// Entry names are compared, and lower-cased for the cipher, with ASCII rules only (the client used std::tolower in the
// "C" locale).
std::string AsciiLower(std::string_view name);

// XORCRC (algorithm 2) and XORCRC3 (algorithm 3): a stream cipher keyed by the lower-cased entry name and the blob
// length. In place. `algorithm` must be kXorCrc or kXorCrc3.
void XorCrcDecrypt(std::span<std::uint8_t> blob, std::string_view name, std::uint32_t algorithm);
void XorCrcEncrypt(std::span<std::uint8_t> blob, std::string_view name, std::uint32_t algorithm);

// Decodes a stored blob of entry `name` into exactly `size` bytes. False and `error` on an unsupported algorithm or
// corrupted data (including a decoded size other than `size`).
bool Decode(std::string_view name, std::uint32_t algorithm, std::span<const std::uint8_t> blob, std::uint32_t size,
            std::vector<std::uint8_t>& out, std::string& error);

} // namespace x2::kom
