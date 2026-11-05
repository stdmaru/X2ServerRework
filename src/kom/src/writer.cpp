#include "x2/kom/writer.h"

#include "codec_internal.h"

#include <cstring>
#include <fstream>
#include <limits>
#include <system_error>
#include <zlib.h>

namespace x2::kom {

namespace {

std::string PathString(const std::filesystem::path& path)
{
    const std::u8string s = path.u8string();
    return std::string(s.begin(), s.end());
}

bool ValidName(std::string_view name, std::string& error)
{
    if (name.empty())
    {
        error = "empty entry name";
        return false;
    }
    for (const char c : name)
    {
        if (static_cast<unsigned char>(c) < 0x20 || c == '/' || c == '\\')
        {
            error = "entry name \"" + std::string(name) + "\" has a path separator or a control character";
            return false;
        }
    }
    if (!platform::IsValidUtf8(name))
    {
        error = "entry name is not UTF-8";
        return false;
    }
    return true;
}

bool Deflate(std::span<const std::uint8_t> data, int level, std::vector<std::uint8_t>& out, std::string& error)
{
    if (data.size() > std::numeric_limits<uLong>::max() / 2)
    {
        error = "entry too large";
        return false;
    }
    uLongf size = compressBound(static_cast<uLong>(data.size()));
    out.resize(size);
    if (compress2(out.data(), &size, data.data(), static_cast<uLong>(data.size()), level) != Z_OK)
    {
        error = "zlib compression failed";
        return false;
    }
    out.resize(size);
    return true;
}

// The client reads offsets and sizes as 32-bit signed values.
constexpr std::uint64_t kMaxArchiveSize = 0x7FFFFFFF;

} // namespace

bool Encode(std::string_view name, std::uint32_t algorithm, std::span<const std::uint8_t> data,
            std::vector<std::uint8_t>& blob, std::string& error, int level)
{
    if (!IsDecodable(algorithm))
    {
        error = "cannot encode with algorithm " + std::to_string(algorithm);
        return false;
    }
    std::vector<std::uint8_t> z;
    if (level < 0 || level > 9)
    {
        error = "zlib level " + std::to_string(level) + " out of range";
        return false;
    }
    if (!Deflate(data, level, z, error))
        return false;
    if (algorithm == kXorCrc3)
    {
        // Prefix: 4 bytes the client never reads; adler32 of the stream without its trailer, as JunePack and kom.py.
        z.resize(z.size() - 4);
        blob.resize(4 + z.size());
        PutU32(blob.data(), Adler32(z));
        std::memcpy(blob.data() + 4, z.data(), z.size());
    }
    else
    {
        blob = std::move(z);
    }
    if (algorithm != kZlib)
        XorCrcEncrypt(blob, name, algorithm);
    return true;
}

ArchiveBuilder ArchiveBuilder::From(std::shared_ptr<Archive> source)
{
    ArchiveBuilder builder;
    for (const Entry& e : source->Entries())
        builder.AddCopy(source, e);
    return builder;
}

void ArchiveBuilder::Put(Item item)
{
    const auto [it, inserted] = index_.try_emplace(AsciiLower(item.entry.name), items_.size());
    if (inserted)
        items_.push_back(std::move(item));
    else
        items_[it->second] = std::move(item);
}

void ArchiveBuilder::AddCopy(std::shared_ptr<Archive> source, const Entry& entry)
{
    Item item;
    item.entry = entry;
    item.sourceIndex = static_cast<std::size_t>(&entry - source->Entries().data());
    item.source = std::move(source);
    Put(std::move(item));
}

bool ArchiveBuilder::Add(std::string name, std::uint32_t algorithm, std::span<const std::uint8_t> data,
                         std::string& error, std::optional<std::uint32_t> fileTime)
{
    if (data.size() > std::numeric_limits<std::uint32_t>::max())
    {
        error = name + ": entry too large";
        return false;
    }
    std::vector<std::uint8_t> blob;
    if (!ValidName(name, error) || !Encode(name, algorithm, data, blob, error, level_))
        return false;
    return AddStored(std::move(name), algorithm, static_cast<std::uint32_t>(data.size()), std::move(blob), error,
                     fileTime);
}

bool ArchiveBuilder::AddStored(std::string name, std::uint32_t algorithm, std::uint32_t size,
                               std::vector<std::uint8_t> blob, std::string& error,
                               std::optional<std::uint32_t> fileTime)
{
    if (!ValidName(name, error))
        return false;
    if (blob.size() > std::numeric_limits<std::uint32_t>::max())
    {
        error = name + ": entry too large";
        return false;
    }
    Item item;
    item.entry.name = std::move(name);
    item.entry.algorithm = algorithm;
    item.entry.size = size;
    item.entry.storedSize = static_cast<std::uint32_t>(blob.size());
    item.entry.checksum = Adler32(blob);
    item.entry.fileTime = fileTime.value_or(item.entry.checksum);
    item.blob = std::move(blob);
    Put(std::move(item));
    return true;
}

bool ArchiveBuilder::Remove(std::string_view name)
{
    const auto it = index_.find(AsciiLower(name));
    if (it == index_.end())
        return false;
    items_.erase(items_.begin() + static_cast<std::ptrdiff_t>(it->second));
    index_.clear();
    for (std::size_t i = 0; i < items_.size(); ++i)
        index_.emplace(AsciiLower(items_[i].entry.name), i);
    return true;
}

bool ArchiveBuilder::Describes(std::span<const std::uint8_t> header) const
{
    if (header.size() < kFixedHeaderSize || std::memcmp(header.data(), kMagic, sizeof kMagic - 1) != 0)
        return false;
    for (std::size_t k = sizeof kMagic - 1; k < kMagicSize; ++k)
        if (header[k] != 0)
            return false;
    const std::uint32_t xmlSize = GetU32(header.data() + 68);
    if (GetU32(header.data() + 52) != items_.size() || GetU32(header.data() + 56) != 1 ||
        header.size() != kFixedHeaderSize + xmlSize)
        return false;
    const auto xml = header.subspan(kFixedHeaderSize);
    if (GetU32(header.data() + 64) != Adler32(xml))
        return false;

    std::vector<Entry> entries;
    std::string error;
    if (!ParseXmlHeader(std::string_view(reinterpret_cast<const char*>(xml.data()), xml.size()), entries, error) ||
        entries.size() != items_.size())
        return false;
    std::uint32_t fileTime = 0;
    for (std::size_t i = 0; i < entries.size(); ++i)
    {
        const Entry& a = entries[i];
        const Entry& b = items_[i].entry;
        if (a.name != b.name || a.size != b.size || a.storedSize != b.storedSize || a.checksum != b.checksum ||
            a.fileTime != b.fileTime || a.algorithm != b.algorithm)
            return false;
        fileTime += a.fileTime;
    }
    return GetU32(header.data() + 60) == fileTime;
}

bool ArchiveBuilder::Write(const std::filesystem::path& destination, std::string& error) const
{
    const std::string where = PathString(destination) + ": ";

    // Unchanged: every entry of one archive, in its order, copied verbatim.
    const Archive* unchanged = nullptr;
    if (!items_.empty() && items_.front().source && items_.size() == items_.front().source->Entries().size())
    {
        unchanged = items_.front().source.get();
        for (std::size_t i = 0; i < items_.size(); ++i)
            if (items_[i].source.get() != unchanged || items_[i].sourceIndex != i)
            {
                unchanged = nullptr;
                break;
            }
    }

    std::vector<std::uint8_t> header;
    if (unchanged)
    {
        const auto raw = unchanged->RawHeader();
        header.assign(raw.begin(), raw.end());
    }
    else if (!header_.empty() && Describes(header_))
    {
        header = header_;
    }
    else
    {
        std::vector<Entry> entries;
        entries.reserve(items_.size());
        std::uint32_t fileTime = 0;
        for (const Item& item : items_)
        {
            entries.push_back(item.entry);
            fileTime += item.entry.fileTime;
        }
        const std::string xml = GenerateXmlHeader(entries);
        header.resize(kFixedHeaderSize + xml.size());
        std::memcpy(header.data(), kMagic, sizeof kMagic - 1);
        PutU32(header.data() + 52, static_cast<std::uint32_t>(items_.size()));
        PutU32(header.data() + 56, 1);
        PutU32(header.data() + 60, fileTime);
        PutU32(header.data() + 64,
               Adler32(std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(xml.data()), xml.size())));
        PutU32(header.data() + 68, static_cast<std::uint32_t>(xml.size()));
        std::memcpy(header.data() + kFixedHeaderSize, xml.data(), xml.size());
    }

    std::uint64_t total = header.size();
    for (const Item& item : items_)
        total += item.entry.storedSize;
    if (total > kMaxArchiveSize)
    {
        error = where + "archive larger than 2 GB, which the client cannot read";
        return false;
    }

    std::filesystem::path temporary = destination;
    temporary += ".tmp";
    std::error_code ec;
    {
        std::ofstream out(temporary, std::ios::binary | std::ios::trunc);
        if (!out)
        {
            error = "cannot create " + PathString(temporary);
            return false;
        }
        out.write(reinterpret_cast<const char*>(header.data()), static_cast<std::streamsize>(header.size()));
        std::vector<std::uint8_t> blob;
        for (const Item& item : items_)
        {
            const std::vector<std::uint8_t>* data = &item.blob;
            if (item.source)
            {
                if (!item.source->ReadStored(item.source->Entries()[item.sourceIndex], blob, error))
                {
                    out.close();
                    std::filesystem::remove(temporary, ec);
                    return false;
                }
                data = &blob;
            }
            out.write(reinterpret_cast<const char*>(data->data()), static_cast<std::streamsize>(data->size()));
        }
        out.flush();
        if (!out)
        {
            out.close();
            std::filesystem::remove(temporary, ec);
            error = "cannot write " + PathString(temporary);
            return false;
        }
    }

    // Release our own handles on the destination (rewriting in place); Windows refuses to replace an open file.
    for (const Item& item : items_)
        if (item.source && item.source->IsOpen() && std::filesystem::equivalent(item.source->Path(), destination, ec))
            item.source->Close();

    std::filesystem::rename(temporary, destination, ec);
    if (ec)
    {
        std::error_code ignored;
        std::filesystem::remove(temporary, ignored);
        error = "cannot replace " + PathString(destination) + ": " + ec.message() +
                " (is it open in another program, such as a running client?)";
        return false;
    }
    return true;
}

} // namespace x2::kom
