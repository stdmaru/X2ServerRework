// Shared helpers of the x2::kom tests: temporary folders and hand-made archives.
#pragma once

#include "codec_internal.h"
#include "x2/kom/writer.h"

#include <doctest/doctest.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace kom_test {

using Bytes = std::vector<std::uint8_t>;

inline Bytes ToBytes(std::string_view s)
{
    return Bytes(s.begin(), s.end());
}

inline Bytes Pattern(std::size_t size, unsigned seed)
{
    Bytes out(size);
    for (std::size_t k = 0; k < size; ++k)
        out[k] = static_cast<std::uint8_t>(seed * 31 + k * 7 + k / 8);
    return out;
}

inline Bytes ReadFile(const std::filesystem::path& path)
{
    std::ifstream in(path, std::ios::binary);
    return Bytes(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

inline void WriteFile(const std::filesystem::path& path, const Bytes& data)
{
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
}

// A fresh folder under the system temporary folder, removed with its contents at the end of the test.
class TempDir
{
public:
    TempDir()
    {
        static std::atomic<unsigned> counter{0};
        const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        path_ = std::filesystem::temp_directory_path() /
                ("x2_kom_test_" + std::to_string(stamp) + "_" + std::to_string(counter++));
        std::filesystem::create_directories(path_);
    }
    ~TempDir()
    {
        std::error_code ec;
        std::filesystem::remove_all(path_, ec);
    }
    const std::filesystem::path& Path() const { return path_; }
    std::filesystem::path operator/(const std::string& name) const { return path_ / name; }

private:
    std::filesystem::path path_;
};

// An archive from a literal XML header and blobs, with the fixed header filled in (count from `count`, or the
// number of blobs).
inline Bytes RawArchive(const std::string& xml, const std::vector<Bytes>& blobs, std::uint32_t fileTime = 0,
                        int count = -1)
{
    using namespace x2::kom;
    Bytes out(kFixedHeaderSize);
    std::memcpy(out.data(), kMagic, sizeof kMagic - 1);
    PutU32(out.data() + 52, count < 0 ? static_cast<std::uint32_t>(blobs.size()) : static_cast<std::uint32_t>(count));
    PutU32(out.data() + 56, 1);
    PutU32(out.data() + 60, fileTime);
    PutU32(out.data() + 64, Adler32(ToBytes(xml)));
    PutU32(out.data() + 68, static_cast<std::uint32_t>(xml.size()));
    out.insert(out.end(), xml.begin(), xml.end());
    for (const Bytes& b : blobs)
        out.insert(out.end(), b.begin(), b.end());
    return out;
}

inline Bytes Encoded(std::string_view name, std::uint32_t algorithm, const Bytes& data,
                     int level = x2::kom::kKomPyLevel)
{
    Bytes blob;
    std::string error;
    REQUIRE_MESSAGE(x2::kom::Encode(name, algorithm, data, blob, error, level), error);
    return blob;
}

} // namespace kom_test
