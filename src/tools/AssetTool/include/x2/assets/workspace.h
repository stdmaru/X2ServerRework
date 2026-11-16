// A client_data folder as the manager sees it (decision T50, docs/tools/client-data.md#managing-client_data): every
// file with its state against the manifest, edits waiting for a sync (the archive of a file), and the operations on
// files. Shared by the x2_assets window and its command line.
//
// Files are found by name, never by path (the manifest holds no paths), so client_data can be reorganized at any
// time; a new file's folder comes from FolderForNew, not from a path a tool assumes.
#pragma once

#include "x2/assets/manifest.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace x2::assets {

enum class FileState
{
    Unchanged,  // as imported
    Modified,   // its content differs from the original entry
    Deleted,    // in the manifest, not in client_data: a sync drops it
    Untracked,  // in client_data, not in the manifest: a sync adds it
    Added,      // in the manifest as a new file (Encoding::New), in no original archive
};

std::string_view ToString(FileState state);

struct ClientDataFile
{
    std::string name;       // the file's name (a kim record's: Item.lua and SetItem.lua, each a file)
    std::string folder;     // relative to client_data, '/'-separated, "" for its root; "" for a deleted file
    FileState state = FileState::Unchanged;
    std::ptrdiff_t record = -1;  // in Manifest().files; -1: untracked
    std::uint64_t size = 0;      // on disk
    std::string sha256;          // of the file on disk
};

struct SyncStats
{
    std::size_t added = 0;    // untracked files given a record
    std::size_t removed = 0;  // records of deleted files dropped
    std::size_t moved = 0;    // records given another archive
};

class ClientData
{
public:
    // Called from worker threads while files are hashed: files hashed so far, of `total`.
    using Progress = std::function<void(std::size_t done, std::size_t total)>;

    // Reads the manifest and every file of `root` (hashes cached by size and time in .kom/hashes.tsv).
    bool Open(const std::filesystem::path& root, unsigned jobs, std::string& error, const Progress& progress = {});
    // Reads the files again; edits of files that are gone are dropped.
    bool Rescan(unsigned jobs, std::string& error, const Progress& progress = {});

    const std::filesystem::path& Root() const { return root_; }
    const Manifest& GetManifest() const { return manifest_; }
    const std::vector<ClientDataFile>& Files() const { return files_; }

    // By name, ASCII case-insensitive; a deleted file too. Null if unknown.
    const ClientDataFile* Find(std::string_view name) const;
    // Where the file is in client_data; empty if it is not there.
    std::filesystem::path Locate(std::string_view name) const;
    // The folder (relative, '/') for a new file: where it already is, else the folder of the first sibling found
    // (a file it belongs with, e.g. a dialog script for UIAnchors.lua), else the root.
    std::string FolderForNew(std::string_view name, std::span<const std::string> siblings = {}) const;

    // The archive a new file of `folder` goes to by default: the most common among the tracked files under that
    // folder, else under its parent, up to the root (ties: the first in load order). Empty if nothing is tracked.
    std::string DefaultArchive(std::string_view folder) const;
    // The archive the file will be in after a sync: the edit, else its record's, else the default of its folder.
    std::string ArchiveOf(const ClientDataFile& file) const;
    bool HasArchiveEdit(const ClientDataFile& file) const;

    // Edits, applied by Sync. The archive must be in the manifest; a deleted file cannot be edited. Choosing the
    // archive the file would get anyway removes the edit.
    bool SetArchive(std::string_view name, std::string_view archive, std::string& error);
    // Every file under `folder` (recursive, "" for all) but the deleted ones; `count` files changed.
    bool SetFolderArchive(std::string_view folder, std::string_view archive, std::size_t& count, std::string& error);
    std::size_t EditCount() const { return edits_.size(); }
    void DiscardEdits() { edits_.clear(); }

    // Writes the manifest: untracked files become new records (in ArchiveOf, with the archive's most common
    // algorithm), deleted files' records are dropped, archive edits are applied. Modified files stay modified (the
    // manifest keeps their original). Then rescans.
    bool Sync(unsigned jobs, SyncStats& stats, std::string& error);

    // On disk, at once (no sync needed to see them, a sync to record them). Folders are relative, '/'.
    bool AddFile(const std::filesystem::path& source, std::string_view folder, std::string& error);  // copies it in
    bool RemoveFile(std::string_view name, std::string& error);
    bool MoveFile(std::string_view name, std::string_view folder, std::string& error);

private:
    std::string RecordKey(const ClientDataFile& file) const;
    bool Hash(unsigned jobs, std::string& error, const Progress& progress);

    std::filesystem::path root_;
    Manifest manifest_;
    std::vector<ClientDataFile> files_;
    std::unordered_map<std::string, std::size_t> byName_;  // AsciiLower(name) -> files_
    std::map<std::string, std::string> edits_;             // AsciiLower(record or file name) -> archive
};

// Where a client_data's manager keeps file hashes between runs.
std::filesystem::path HashCachePath(const std::filesystem::path& clientData);

} // namespace x2::assets
