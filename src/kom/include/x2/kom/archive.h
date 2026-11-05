// Reading .kom archives ("KOG GC TEAM MASSFILE V.0.3."), decision T35, docs/porting/kom.md.
//
//   0   char[52]  magic, NUL padded
//   52  u32       number of entries
//   56  u32       compressed flag (1)
//   60  u32       file time (sum of the entries' FileTime)
//   64  u32       adler32 of the XML header
//   68  u32       XML header size
//   72  XML       <Files><File Name= Size= CompressedSize= Checksum= FileTime= Algorithm= />...</Files>
//   ..  data      entry blobs, back to back, in XML order
//
// Size is the decoded size, CompressedSize the stored blob size, Checksum the adler32 of the stored blob.
#pragma once

#include "x2/kom/codec.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace x2::platform {
class ReadOnlyFile;
}

namespace x2::kom {

struct Entry
{
    std::string name;            // as in the XML header (UTF-8); entry names have no folders
    std::uint32_t size = 0;      // decoded size
    std::uint32_t storedSize = 0;
    std::uint32_t checksum = 0;  // adler32 of the stored blob
    std::uint32_t fileTime = 0;  // KOG's packer stores the checksum here too
    std::uint32_t algorithm = kZlib;
    std::uint64_t offset = 0;    // of the stored blob, from the start of the archive
};

class Archive
{
public:
    // Reads the fixed header and the XML header only; entry data is read on demand. Rejects anything that is not a
    // well-formed V.0.3 archive: other magic or version, XML header checksum mismatch, entry count different from the
    // XML, entries past the end of the file.
    static std::unique_ptr<Archive> Open(const std::filesystem::path& path, std::string& error);

    ~Archive();
    Archive(const Archive&) = delete;
    Archive& operator=(const Archive&) = delete;

    const std::filesystem::path& Path() const { return path_; }
    const std::vector<Entry>& Entries() const { return entries_; }
    std::uint32_t FileTime() const { return fileTime_; }

    // First entry with that name, compared case-insensitively (ASCII); nullptr if none.
    const Entry* Find(std::string_view name) const;

    // The stored blob, after checking its adler32 against the entry's checksum. Thread-safe (positional reads).
    bool ReadStored(const Entry& entry, std::vector<std::uint8_t>& out, std::string& error) const;

    // The decoded data. Fails for algorithm 1, which is kept opaque.
    bool Read(const Entry& entry, std::vector<std::uint8_t>& out, std::string& error) const;

    // The fixed header and the XML header exactly as stored, for rewriting an unchanged archive byte for byte.
    std::span<const std::uint8_t> RawHeader() const { return rawHeader_; }

    // Releases the file (entries stay listable; reads then fail). Not concurrent with reads. The writer calls it
    // before replacing this archive's file (writer.h).
    void Close();
    bool IsOpen() const { return file_ != nullptr; }

private:
    Archive() = default;

    std::filesystem::path path_;
    std::unique_ptr<platform::ReadOnlyFile> file_;
    std::vector<Entry> entries_;
    std::unordered_map<std::string, std::size_t> index_;  // AsciiLower(name) -> first entry
    std::vector<std::uint8_t> rawHeader_;
    std::uint32_t fileTime_ = 0;
};

} // namespace x2::kom
