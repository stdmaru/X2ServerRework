#include "x2/kom/archive.h"

#include "codec_internal.h"
#include "x2/platform/file.h"

#include <cstring>

namespace x2::kom {

namespace {

std::string PathString(const std::filesystem::path& path)
{
    const std::u8string s = path.u8string();
    return std::string(s.begin(), s.end());
}

} // namespace

std::unique_ptr<Archive> Archive::Open(const std::filesystem::path& path, std::string& error)
{
    const std::string where = PathString(path) + ": ";
    std::string ioError;
    auto file = platform::ReadOnlyFile::Open(path, ioError);
    if (!file)
    {
        error = ioError;
        return nullptr;
    }

    std::uint8_t fixed[kFixedHeaderSize];
    if (file->Size() < kFixedHeaderSize || !file->ReadAt(0, fixed, sizeof fixed, ioError))
    {
        error = where + "not a .kom archive (too short)";
        return nullptr;
    }
    const std::size_t magicLength = sizeof kMagic - 1;
    const bool zeroPadded = [&] {
        for (std::size_t i = magicLength; i < kMagicSize; ++i)
            if (fixed[i] != 0)
                return false;
        return true;
    }();
    if (std::memcmp(fixed, kMagic, magicLength) != 0 || !zeroPadded)
    {
        if (std::memcmp(fixed, "KOG GC TEAM MASSFILE V.0.", 25) == 0)
            error = where + "unsupported .kom version " + std::string(reinterpret_cast<const char*>(fixed) + 21, 6);
        else
            error = where + "not a .kom archive";
        return nullptr;
    }
    const std::uint32_t count = GetU32(fixed + 52);
    const std::uint32_t compressed = GetU32(fixed + 56);
    const std::uint32_t fileTime = GetU32(fixed + 60);
    const std::uint32_t xmlAdler = GetU32(fixed + 64);
    const std::uint32_t xmlSize = GetU32(fixed + 68);
    if (compressed != 1)
    {
        error = where + "unsupported compressed flag " + std::to_string(compressed);
        return nullptr;
    }
    if (kFixedHeaderSize + std::uint64_t{xmlSize} > file->Size())
    {
        error = where + "truncated XML header";
        return nullptr;
    }

    std::unique_ptr<Archive> archive(new Archive);
    archive->rawHeader_.resize(kFixedHeaderSize + xmlSize);
    std::memcpy(archive->rawHeader_.data(), fixed, kFixedHeaderSize);
    if (!file->ReadAt(kFixedHeaderSize, archive->rawHeader_.data() + kFixedHeaderSize, xmlSize, ioError))
    {
        error = where + ioError;
        return nullptr;
    }
    const auto xml = std::span<const std::uint8_t>(archive->rawHeader_).subspan(kFixedHeaderSize);
    if (Adler32(xml) != xmlAdler)
    {
        error = where + "XML header checksum mismatch";
        return nullptr;
    }
    std::string parseError;
    if (!ParseXmlHeader(std::string_view(reinterpret_cast<const char*>(xml.data()), xml.size()), archive->entries_,
                        parseError))
    {
        error = where + "bad XML header: " + parseError;
        return nullptr;
    }
    if (archive->entries_.size() != count)
    {
        error = where + "the header says " + std::to_string(count) + " entries, the XML lists " +
                std::to_string(archive->entries_.size());
        return nullptr;
    }

    std::uint64_t offset = kFixedHeaderSize + std::uint64_t{xmlSize};
    for (std::size_t i = 0; i < archive->entries_.size(); ++i)
    {
        Entry& e = archive->entries_[i];
        e.offset = offset;
        offset += e.storedSize;
        if (offset > file->Size())
        {
            error = where + "entry " + e.name + " is past the end of the file";
            return nullptr;
        }
        archive->index_.emplace(AsciiLower(e.name), i);
    }

    archive->path_ = path;
    archive->fileTime_ = fileTime;
    archive->file_ = std::move(file);
    return archive;
}

Archive::~Archive() = default;

const Entry* Archive::Find(std::string_view name) const
{
    const auto it = index_.find(AsciiLower(name));
    return it == index_.end() ? nullptr : &entries_[it->second];
}

bool Archive::ReadStored(const Entry& entry, std::vector<std::uint8_t>& out, std::string& error) const
{
    if (!file_)
    {
        error = PathString(path_) + ": archive closed";
        return false;
    }
    out.resize(entry.storedSize);
    std::string ioError;
    if (!file_->ReadAt(entry.offset, out.data(), out.size(), ioError))
    {
        error = PathString(path_) + ": " + entry.name + ": " + ioError;
        return false;
    }
    if (Adler32(out) != entry.checksum)
    {
        error = PathString(path_) + ": " + entry.name + ": checksum mismatch";
        return false;
    }
    return true;
}

bool Archive::Read(const Entry& entry, std::vector<std::uint8_t>& out, std::string& error) const
{
    std::vector<std::uint8_t> blob;
    if (!ReadStored(entry, blob, error))
        return false;
    std::string decodeError;
    if (!Decode(entry.name, entry.algorithm, blob, entry.size, out, decodeError))
    {
        error = PathString(path_) + ": " + entry.name + ": " + decodeError;
        return false;
    }
    return true;
}

void Archive::Close()
{
    file_.reset();
}

} // namespace x2::kom
