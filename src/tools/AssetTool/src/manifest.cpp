#include "x2/assets/manifest.h"

#include <charconv>
#include <fstream>
#include <sstream>

namespace x2::assets {

namespace {

constexpr std::string_view kMagic = "x2-assets-manifest";

std::vector<std::string_view> SplitTabs(std::string_view line)
{
    std::vector<std::string_view> fields;
    std::size_t start = 0;
    for (;;)
    {
        const std::size_t tab = line.find('\t', start);
        fields.push_back(line.substr(start, tab == std::string_view::npos ? std::string_view::npos : tab - start));
        if (tab == std::string_view::npos)
            return fields;
        start = tab + 1;
    }
}

bool ParseNumber(std::string_view text, int base, std::uint32_t& value)
{
    const auto [end, ec] = std::from_chars(text.data(), text.data() + text.size(), value, base);
    return ec == std::errc() && end == text.data() + text.size() && !text.empty();
}

std::string Hex8(std::uint32_t v)
{
    static constexpr char kDigits[] = "0123456789abcdef";
    std::string out(8, '0');
    for (int i = 7; i >= 0; --i, v >>= 4)
        out[static_cast<std::size_t>(i)] = kDigits[v & 0xF];
    return out;
}

bool ParseEncoding(std::string_view text, Encoding& encoding)
{
    for (const Encoding e : {Encoding::Zlib, Encoding::Blob, Encoding::Stored})
        if (text == ToString(e))
        {
            encoding = e;
            return true;
        }
    return false;
}

bool IsSha256Hex(std::string_view s)
{
    if (s.size() != 64)
        return false;
    for (const char c : s)
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')))
            return false;
    return true;
}

} // namespace

std::string_view ToString(Encoding encoding)
{
    switch (encoding)
    {
    case Encoding::Zlib: return "zlib";
    case Encoding::Blob: return "blob";
    case Encoding::Stored: return "stored";
    }
    return "?";
}

std::filesystem::path PathFromUtf8(std::string_view s)
{
    return std::filesystem::path(std::u8string(s.begin(), s.end()));
}

std::string PathToUtf8(const std::filesystem::path& path)
{
    const std::u8string s = path.u8string();
    return std::string(s.begin(), s.end());
}

std::filesystem::path ManifestPath(const std::filesystem::path& clientData)
{
    return clientData / kMetaFolder / "manifest.tsv";
}

std::filesystem::path HeaderPath(const std::filesystem::path& clientData, std::string_view archive)
{
    return clientData / kMetaFolder / "headers" / PathFromUtf8(std::string(archive) + ".hdr");
}

std::filesystem::path BlobPath(const std::filesystem::path& clientData, std::string_view name)
{
    return clientData / kMetaFolder / "blobs" / PathFromUtf8(name);
}

bool Manifest::Read(const std::filesystem::path& path, std::string& error)
{
    archives.clear();
    files.clear();
    std::ifstream in(path, std::ios::binary);
    if (!in)
    {
        error = "cannot open " + PathToUtf8(path);
        return false;
    }
    const std::string where = PathToUtf8(path) + ":";
    bool sawVersion = false;
    std::string line;
    for (int number = 1; std::getline(in, line); ++number)
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (line.empty() || line.front() == '#')
            continue;
        const auto bad = [&](const std::string& what) {
            error = where + std::to_string(number) + ": " + what;
            return false;
        };
        const auto f = SplitTabs(line);
        if (f[0] == kMagic)
        {
            std::uint32_t version = 0;
            if (f.size() != 2 || !ParseNumber(f[1], 10, version))
                return bad("bad version line");
            if (version != kVersion)
                return bad("manifest version " + std::to_string(version) + " is not supported (expected " +
                           std::to_string(kVersion) + ")");
            sawVersion = true;
            continue;
        }
        if (!sawVersion)
            return bad("not an x2-assets manifest");
        if (f[0] == "archive")
        {
            ArchiveRecord a;
            if (f.size() != 3 || f[1].empty() || !ParseNumber(f[2], 10, a.count))
                return bad("bad archive record");
            a.name = std::string(f[1]);
            archives.push_back(std::move(a));
        }
        else if (f[0] == "file")
        {
            FileRecord r;
            if (f.size() != 11 || f[1].empty() || f[2].empty() || !ParseNumber(f[3], 10, r.index) ||
                !ParseNumber(f[4], 10, r.algorithm) || !ParseNumber(f[5], 10, r.size) ||
                !ParseNumber(f[6], 10, r.storedSize) || f[7].size() != 8 || !ParseNumber(f[7], 16, r.checksum) ||
                f[8].size() != 8 || !ParseNumber(f[8], 16, r.fileTime) || !ParseEncoding(f[9], r.encoding) ||
                !IsSha256Hex(f[10]))
                return bad("bad file record");
            r.name = std::string(f[1]);
            r.archive = std::string(f[2]);
            r.sha256 = std::string(f[10]);
            files.push_back(std::move(r));
        }
        else
        {
            return bad("unknown record \"" + std::string(f[0]) + "\"");
        }
    }
    if (!sawVersion)
    {
        error = where + " empty manifest";
        return false;
    }
    return true;
}

bool Manifest::Write(const std::filesystem::path& path, std::string& error) const
{
    std::ostringstream out;
    out << "# client_data manifest (docs/tools/client-data.md): packs this folder back into the client's .kom\n"
        << kMagic << '\t' << kVersion << '\n';
    for (const ArchiveRecord& a : archives)
        out << "archive\t" << a.name << '\t' << a.count << '\n';
    for (const FileRecord& r : files)
        out << "file\t" << r.name << '\t' << r.archive << '\t' << r.index << '\t' << r.algorithm << '\t' << r.size
            << '\t' << r.storedSize << '\t' << Hex8(r.checksum) << '\t' << Hex8(r.fileTime) << '\t'
            << ToString(r.encoding) << '\t' << r.sha256 << '\n';
    const std::string text = out.str();

    std::filesystem::path temporary = path;
    temporary += ".tmp";
    {
        std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
        file.write(text.data(), static_cast<std::streamsize>(text.size()));
        file.flush();
        if (!file)
        {
            error = "cannot write " + PathToUtf8(temporary);
            return false;
        }
    }
    std::error_code ec;
    std::filesystem::rename(temporary, path, ec);
    if (ec)
    {
        error = "cannot replace " + PathToUtf8(path) + ": " + ec.message();
        return false;
    }
    return true;
}

} // namespace x2::assets
