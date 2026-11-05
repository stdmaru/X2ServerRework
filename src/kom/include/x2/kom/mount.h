// A set of .kom archives looked up by entry name, as the client's KGCMassFileManager (decision T35,
// docs/porting/kom.md#mounting).
#pragma once

#include "x2/kom/archive.h"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace x2::kom {

using WarningSink = std::function<void(const std::string&)>;

// Writes "kom: warning: <message>" to stderr.
void DefaultWarningSink(const std::string& message);

// The .kom files of a folder in the client's load order: file names sorted case-insensitively (ASCII), which is the
// original's order data001..data150 then datai001..datai023. Not recursive. False and `error` if the folder cannot be
// listed.
bool ArchivesInLoadOrder(const std::filesystem::path& folder, std::vector<std::filesystem::path>& out,
                         std::string& error);

class Mount
{
public:
    explicit Mount(WarningSink warn = DefaultWarningSink);

    // Adds an archive after the ones already mounted. Names are case-insensitive (ASCII); a name already mounted (from
    // an earlier archive, or earlier in the same one) keeps its first entry, and the repeat is reported to the
    // warning sink. False and `error` if the archive cannot be opened (nothing is added).
    bool AddArchive(const std::filesystem::path& path, std::string& error);
    bool AddArchive(std::shared_ptr<Archive> archive);

    // Adds every .kom of a folder in load order (ArchivesInLoadOrder). Stops at the first archive that cannot be
    // opened.
    bool AddFolder(const std::filesystem::path& folder, std::string& error);

    struct Hit
    {
        const Archive* archive = nullptr;
        const Entry* entry = nullptr;
        explicit operator bool() const { return entry != nullptr; }
    };

    // Thread-safe once mounting is done (the map is not modified by lookups).
    Hit Find(std::string_view name) const;
    bool Read(std::string_view name, std::vector<std::uint8_t>& out, std::string& error) const;

    const std::vector<std::shared_ptr<Archive>>& Archives() const { return archives_; }
    std::size_t NameCount() const { return names_.size(); }

private:
    WarningSink warn_;
    std::vector<std::shared_ptr<Archive>> archives_;
    std::unordered_map<std::string, Hit> names_;  // AsciiLower(name)
};

} // namespace x2::kom
