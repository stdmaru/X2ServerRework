// Writing .kom archives (decision T35, docs/porting/kom.md#writing): tools and the launcher only, in the separate
// x2::kom_write target so the client does not carry it.
#pragma once

#include "x2/kom/archive.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace x2::kom {

// zlib levels: kom.py compresses with 9; KOG's packer used 6, zlib's default (with zlib 1.3.1, 99.9% of the
// client's entries re-encode byte for byte at level 6, none at 9; docs/porting/kom.md#writing).
inline constexpr int kKomPyLevel = 9;
inline constexpr int kKogLevel = 6;

// Encodes data as an entry blob with algorithm 0, 2 or 3.
bool Encode(std::string_view name, std::uint32_t algorithm, std::span<const std::uint8_t> data,
            std::vector<std::uint8_t>& blob, std::string& error, int level = kKomPyLevel);

// Builds an archive from entries copied verbatim from open archives and new data.
//
// Entries keep the order they were added in; adding a name that is already present (case-insensitively) replaces
// that entry in place. An archive rewritten with no changes (every entry of one archive, in its order, copied
// verbatim) is written byte for byte as it was, header included; so is a header given to SetHeader that still
// describes the entries exactly. Otherwise the XML header is generated in the layout of KOG's packer
// (GenerateXmlHeader), deterministically.
class ArchiveBuilder
{
public:
    ArchiveBuilder() = default;

    // A builder holding every entry of `source`, in order.
    static ArchiveBuilder From(std::shared_ptr<Archive> source);

    // Copies one entry's stored blob verbatim (any algorithm, including the opaque 1).
    void AddCopy(std::shared_ptr<Archive> source, const Entry& entry);

    // zlib level used by Add (kKomPyLevel unless set).
    void SetLevel(int level) { level_ = level; }

    // A header (fixed + XML, as Archive::RawHeader) to write instead of a generated one when it describes this
    // builder's entries exactly: same count and order, names, sizes, checksums, times and algorithms, and a valid
    // fixed header. Lets a tool that keeps an archive's original header reproduce it from re-encoded entries.
    void SetHeader(std::vector<std::uint8_t> header) { header_ = std::move(header); }

    // Whether `header` describes the entries added so far exactly (the test SetHeader applies).
    bool Describes(std::span<const std::uint8_t> header) const;

    // Encodes `data`. fileTime defaults to the blob's checksum, as KOG's packer does.
    bool Add(std::string name, std::uint32_t algorithm, std::span<const std::uint8_t> data, std::string& error,
             std::optional<std::uint32_t> fileTime = std::nullopt);

    // An already encoded blob (e.g. an opaque algorithm 1 entry kept by an extraction tool). `size` is its decoded size.
    bool AddStored(std::string name, std::uint32_t algorithm, std::uint32_t size, std::vector<std::uint8_t> blob,
                   std::string& error, std::optional<std::uint32_t> fileTime = std::nullopt);

    bool Remove(std::string_view name);
    std::size_t Count() const { return items_.size(); }

    // Writes to `<destination>.tmp` in the same folder and renames it over `destination`. Blobs copied from archives
    // are checked against their checksums while copying. Source archives whose file is `destination` are closed
    // (Archive::Close) just before the rename, so an archive can be rewritten in place; reopen it afterwards. Fails
    // clearly, leaving `destination` untouched, when it cannot be replaced, e.g. on Windows while a running client has
    // it open.
    bool Write(const std::filesystem::path& destination, std::string& error) const;

private:
    struct Item
    {
        Entry entry;                       // offset unused
        std::shared_ptr<Archive> source;   // copy from source at sourceIndex, or
        std::size_t sourceIndex = 0;
        std::vector<std::uint8_t> blob;    // new blob when source is null
    };

    void Put(Item item);

    std::vector<Item> items_;
    std::unordered_map<std::string, std::size_t> index_;  // AsciiLower(name) -> items_
    std::vector<std::uint8_t> header_;                    // SetHeader
    int level_ = kKomPyLevel;
};

} // namespace x2::kom
