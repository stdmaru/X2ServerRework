// Internal helpers of x2::kom shared by the reader and the writer.
#pragma once

#include "x2/kom/archive.h"
#include "x2/platform/unicode.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace x2::kom {

inline std::u16string Utf8ToUtf16(std::string_view s)
{
    return platform::WideToUtf16(platform::Utf8ToWide(s));
}

// Fixed part of a V.0.3 archive: magic (52 bytes, NUL padded), then five little-endian u32: entry count,
// compressed flag (1), file time (sum of the entries' FileTime), adler32 of the XML header, XML header size.
inline constexpr char kMagic[] = "KOG GC TEAM MASSFILE V.0.3.";
inline constexpr std::size_t kMagicSize = 52;
inline constexpr std::size_t kFixedHeaderSize = 72;

inline void PutU32(std::uint8_t* p, std::uint32_t v)
{
    for (int i = 0; i < 4; ++i)
        p[i] = static_cast<std::uint8_t>(v >> (8 * i));
}

inline std::uint32_t GetU32(const std::uint8_t* p)
{
    return static_cast<std::uint32_t>(p[0]) | static_cast<std::uint32_t>(p[1]) << 8 |
           static_cast<std::uint32_t>(p[2]) << 16 | static_cast<std::uint32_t>(p[3]) << 24;
}

// Parses the XML header into entries (name, sizes, checksum, time, algorithm; offsets are left at 0). Accepts both
// packer layouts seen in the client's files (docs/porting/kom.md#xml-header).
bool ParseXmlHeader(std::string_view xml, std::vector<Entry>& entries, std::string& error);

// The XML header as KOG's packer writes it: `<?xml version="1.0"?><Files><File Name="..." Size="..."
// CompressedSize="..." Checksum="%08x" FileTime="%08x" Algorithm="..." />...</Files>`.
std::string GenerateXmlHeader(const std::vector<Entry>& entries);

} // namespace x2::kom
