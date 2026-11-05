// Shared helpers of the x2::assets tests: temporary folders, files and hand-made archives.
#pragma once

#include "x2/kom/writer.h"

#include <doctest/doctest.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace assets_test {

using Bytes = std::vector<std::uint8_t>;

inline Bytes ToBytes(std::string_view s)
{
    return Bytes(s.begin(), s.end());
}

// Compressible, not trivially so: zlib levels give different streams for it.
inline Bytes Pattern(std::size_t size, unsigned seed)
{
    Bytes out(size);
    std::uint32_t x = seed * 2654435761u + 1;
    for (std::size_t k = 0; k < size; ++k)
    {
        x = x * 1103515245u + 12345u;
        out[k] = static_cast<std::uint8_t>("abcdefgh  \n"[(x >> 16) % 11] + (k % 64 == 0 ? 1 : 0));
    }
    return out;
}

inline Bytes ReadFile(const std::filesystem::path& path)
{
    std::ifstream in(path, std::ios::binary);
    return Bytes(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

inline void WriteFile(const std::filesystem::path& path, const Bytes& data)
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
}

// A fresh folder under the system temporary folder (or `base`), removed with its contents at the end of the test.
class TempDir
{
public:
    explicit TempDir(std::filesystem::path base = std::filesystem::temp_directory_path())
    {
        static std::atomic<unsigned> counter{0};
        const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        path_ = base / ("x2_assets_test_" + std::to_string(stamp) + "_" + std::to_string(counter++));
        std::filesystem::create_directories(path_);
    }
    ~TempDir()
    {
        std::error_code ec;
        std::filesystem::remove_all(path_, ec);
    }
    TempDir(const TempDir&) = delete;
    TempDir& operator=(const TempDir&) = delete;
    const std::filesystem::path& Path() const { return path_; }
    std::filesystem::path operator/(const std::string& name) const { return path_ / name; }

private:
    std::filesystem::path path_;
};

inline void PutU32(Bytes& out, std::size_t at, std::uint32_t v)
{
    for (int i = 0; i < 4; ++i)
        out[at + static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(v >> (8 * i));
}

// An archive from a literal XML header and blobs, with the fixed header filled in.
inline Bytes RawArchive(const std::string& xml, const std::vector<Bytes>& blobs, std::uint32_t fileTime)
{
    Bytes out(72, 0);
    std::memcpy(out.data(), "KOG GC TEAM MASSFILE V.0.3.", 27);
    PutU32(out, 52, static_cast<std::uint32_t>(blobs.size()));
    PutU32(out, 56, 1);
    PutU32(out, 60, fileTime);
    PutU32(out, 64, x2::kom::Adler32(ToBytes(xml)));
    PutU32(out, 68, static_cast<std::uint32_t>(xml.size()));
    out.insert(out.end(), xml.begin(), xml.end());
    for (const Bytes& b : blobs)
        out.insert(out.end(), b.begin(), b.end());
    return out;
}

inline Bytes Encoded(std::string_view name, std::uint32_t algorithm, const Bytes& data, int level)
{
    Bytes blob;
    std::string error;
    REQUIRE_MESSAGE(x2::kom::Encode(name, algorithm, data, blob, error, level), error);
    return blob;
}

} // namespace assets_test
