#include "x2/assets/texture_sizes.h"

#include "x2/kom/codec.h"
#include "x2/kom/mount.h"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <fstream>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

namespace x2::assets {

namespace {

std::uint32_t Le16(const std::uint8_t* p) { return p[0] | (p[1] << 8); }
std::uint32_t Le32(const std::uint8_t* p) { return Le16(p) | (Le16(p + 2) << 16); }
std::uint32_t Be16(const std::uint8_t* p) { return (p[0] << 8) | p[1]; }
std::uint32_t Be32(const std::uint8_t* p) { return (Be16(p) << 16) | Be16(p + 2); }

bool JpegSize(const std::uint8_t* data, std::size_t size, std::uint32_t& width, std::uint32_t& height)
{
    std::size_t i = 2;
    while (i + 4 <= size)
    {
        if (data[i] != 0xFF)
            return false;
        const std::uint8_t marker = data[i + 1];
        if (marker == 0xFF)
        {
            ++i;
            continue;
        }
        if (marker == 0xD8 || (marker >= 0xD0 && marker <= 0xD7) || marker == 0x01)
        {
            i += 2;
            continue;
        }
        const std::size_t length = Be16(data + i + 2);
        // Start of frame: every SOFn but DHT (C4), JPG (C8) and DAC (CC).
        if (marker >= 0xC0 && marker <= 0xCF && marker != 0xC4 && marker != 0xC8 && marker != 0xCC)
        {
            if (i + 9 > size)
                return false;
            height = Be16(data + i + 5);
            width = Be16(data + i + 7);
            return width != 0 && height != 0;
        }
        i += 2 + length;
    }
    return false;
}

bool IsImageName(const std::string& lower)
{
    for (const char* ext : {".dds", ".tga", ".bmp", ".png", ".jpg", ".jpeg"})
    {
        const std::size_t n = std::strlen(ext);
        if (lower.size() > n && lower.compare(lower.size() - n, n, ext) == 0)
            return true;
    }
    return false;
}

} // namespace

bool ImageSize(const std::uint8_t* data, std::size_t size, std::uint32_t& width, std::uint32_t& height)
{
    if (size >= 128 && std::memcmp(data, "DDS ", 4) == 0)
    {
        height = Le32(data + 12);
        width = Le32(data + 16);
        return width != 0 && height != 0;
    }
    if (size >= 26 && data[0] == 'B' && data[1] == 'M')
    {
        width = Le32(data + 18);
        const auto h = static_cast<std::int32_t>(Le32(data + 22));  // negative: top-down
        height = static_cast<std::uint32_t>(h < 0 ? -h : h);
        return width != 0 && height != 0;
    }
    if (size >= 24 && std::memcmp(data, "\x89PNG\r\n\x1a\n", 8) == 0)
    {
        width = Be32(data + 16);
        height = Be32(data + 20);
        return width != 0 && height != 0;
    }
    if (size >= 4 && data[0] == 0xFF && data[1] == 0xD8)
        return JpegSize(data, size, width, height);
    if (size >= 18)
    {
        const std::uint8_t mapType = data[1], type = data[2], bits = data[16];
        const bool knownType = type == 1 || type == 2 || type == 3 || type == 9 || type == 10 || type == 11;
        const bool knownBits = bits == 8 || bits == 15 || bits == 16 || bits == 24 || bits == 32;
        if (mapType <= 1 && knownType && knownBits)
        {
            width = Le16(data + 12);
            height = Le16(data + 14);
            return width != 0 && height != 0;
        }
    }
    return false;
}

bool WriteTextureSizes(const std::filesystem::path& source, const std::filesystem::path& output, unsigned jobs,
                       TextureSizeStats& stats, std::string& error)
{
    x2::kom::Mount mount([](const std::string&) {});  // repeated names: first wins, as the client
    if (!mount.AddFolder(source, error))
        return false;

    // Every image name once, the entry the client loads (first mounted).
    std::vector<std::pair<const x2::kom::Archive*, const x2::kom::Entry*>> entries;
    std::unordered_set<std::string> seen;
    for (const auto& archive : mount.Archives())
        for (const auto& entry : archive->Entries())
        {
            std::string lower = x2::kom::AsciiLower(entry.name);
            if (IsImageName(lower) && seen.insert(std::move(lower)).second)
                entries.emplace_back(archive.get(), &entry);
        }

    struct Line
    {
        std::string name;
        std::uint32_t width = 0, height = 0;
        bool ok = false;
    };
    std::vector<Line> lines(entries.size());
    std::atomic<std::size_t> next{0};
    std::mutex errorLock;
    std::string firstError;
    const auto work = [&] {
        std::vector<std::uint8_t> data;
        std::string readError;
        for (std::size_t i; (i = next++) < entries.size();)
        {
            const auto& [archive, entry] = entries[i];
            lines[i].name = entry->name;
            if (!archive->Read(*entry, data, readError))
            {
                std::lock_guard lock(errorLock);
                if (firstError.empty())
                    firstError = readError;
                continue;
            }
            lines[i].ok = ImageSize(data.data(), data.size(), lines[i].width, lines[i].height);
        }
    };
    if (jobs == 0)
        jobs = std::max(1u, std::thread::hardware_concurrency());
    std::vector<std::thread> threads;
    for (unsigned t = 1; t < jobs; ++t)
        threads.emplace_back(work);
    work();
    for (auto& thread : threads)
        thread.join();
    if (!firstError.empty())
    {
        error = firstError;
        return false;
    }

    std::sort(lines.begin(), lines.end(), [](const Line& a, const Line& b) {
        return x2::kom::AsciiLower(a.name) < x2::kom::AsciiLower(b.name);
    });
    std::ofstream out(output, std::ios::binary);
    if (!out)
    {
        error = "cannot write " + output.string();
        return false;
    }
    out << "# The original size of every image in the client's archives, by name: the size its .tet keys and the\n"
           "# dialogs were made for. A texture whose file is larger (upscaled) is drawn at this size; edit a line when\n"
           "# a texture is redrawn with new .tet rectangles. Written by x2_assets texture-sizes\n"
           "# (docs/tools/client-data.md#original-texture-sizes).\n";
    for (const Line& line : lines)
    {
        if (!line.ok)
        {
            ++stats.skipped;
            continue;
        }
        out << line.name << '\t' << line.width << '\t' << line.height << '\n';
        ++stats.images;
    }
    if (!out)
    {
        error = "cannot write " + output.string();
        return false;
    }
    return true;
}

} // namespace x2::assets
