// A client_data folder as the manager sees it (decision T50, docs/tools/client-data.md#managing-client_data): every
// file with its state against the manifest, edits waiting for a sync (the archive of a file), and the operations on
// files. Shared by the x2_assets window and its command line.
//
// Files are found by name, never by path (the manifest holds no paths), so client_data can be reorganized at any
// time; a new file's folder comes from FolderForNew, not from a path a tool assumes.
#pragma once

#include "x2/assets/client_data.h"
#include "x2/assets/manifest.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <set>
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
    Ignored,    // in client_data, not in the manifest, kept out by .kom/ignore.txt (IgnoreList)
};

std::string_view ToString(FileState state);

struct ClientDataFile
{
    std::string name;       // the file's name (a kim record's: Item.lua and SetItem.lua, each a file)
    std::string folder;     // relative to client_data, '/'-separated, "" for its root; "" for a deleted file
    FileState state = FileState::Unchanged;
    std::ptrdiff_t record = -1;  // in Manifest().files; -1: untracked
    std::uint64_t size = 0;      // on disk
    std::int64_t time = 0;       // on disk (last write), for the hash cache
    std::string sha256;          // of the file on disk
};

struct SyncStats
{
    std::size_t added = 0;    // untracked files given a record
    std::size_t removed = 0;  // records of deleted files dropped
    std::size_t moved = 0;    // records given another archive
    std::size_t archives = 0; // archives created
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
    // Every folder of client_data (relative, '/'), empty ones included.
    const std::set<std::string>& Folders() const { return folders_; }

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

    // The archives of the manifest in load order, then the ones created since the last sync.
    std::vector<std::string> Archives() const;
    // A new archive, created by the next sync (and written by pack, with a generated header): a file name ending in
    // .kom that no archive has. Files get it through SetArchive.
    bool CreateArchive(std::string_view name, std::string& error);

    // Edits, applied by Sync. The archive must be in Archives(); a deleted or ignored file cannot be edited. Choosing
    // the archive the file would get anyway removes the edit.
    bool SetArchive(std::string_view name, std::string_view archive, std::string& error);
    // Every file under `folder` (recursive, "" for all) but the deleted ones; `count` files changed.
    bool SetFolderArchive(std::string_view folder, std::string_view archive, std::size_t& count, std::string& error);
    std::size_t EditCount() const { return edits_.size() + newArchives_.size(); }
    void DiscardEdits()
    {
        edits_.clear();
        newArchives_.clear();
    }
    // The edits as a value, to put them back (undo).
    struct Edits
    {
        std::map<std::string, std::string> archives;
        std::vector<std::string> newArchives;
    };
    Edits GetEdits() const { return {edits_, newArchives_}; }
    void SetEdits(Edits edits)
    {
        edits_ = std::move(edits.archives);
        newArchives_ = std::move(edits.newArchives);
    }

    // .kom/ignore.txt, written at once. Ignore(file) adds the file's own path (a name not in client_data yet: the
    // name, matching it in any folder), IgnoreFolder the folder, IgnorePattern any pattern (IgnoreList); Unignore
    // drops every pattern that matches the file (false if one is not its own path: `error` names it). Patterns only
    // keep out files the manifest does not hold.
    const IgnoreList& Ignores() const { return ignore_; }
    bool Ignore(std::string_view name, std::string& error);
    bool IgnoreFolder(std::string_view folder, std::string& error);
    bool IgnorePattern(std::string_view pattern, std::string& error);
    bool Unignore(std::string_view name, std::string& error);
    bool SetIgnores(IgnoreList list, std::string& error);  // all of them (undo)

    // Writes the manifest: untracked files become new records (in ArchiveOf, with the archive's most common
    // algorithm), deleted files' records are dropped, archive edits are applied. Modified files stay modified (the
    // manifest keeps their original). The previous manifest is kept as manifest.tsv.bak.
    bool Sync(SyncStats& stats, std::string& error);

    // Makes client_data's content the original (.kom are the source of truth, client_data the workspace): a Sync,
    // then the manifest takes every file as it is now (modified and new files become originals, encoded as a pack
    // would; deleted ones leave it). Nothing is written but the manifest (previous one: manifest.tsv.bak) and
    // .kom/originals; afterwards client_data shows no changes and a pack writes these contents.
    struct CommitStats
    {
        SyncStats sync;
        std::size_t committed = 0;  // records that took the file's content
    };
    bool Commit(unsigned jobs, CommitStats& stats, std::string& error);

    // On disk, at once, and in this model (no rescan needed to see them, a sync to record them). Folders are
    // relative, '/'. A new name keeps the file's or folder's when empty.
    bool AddFile(const std::filesystem::path& source, std::string_view folder, std::string& error);  // copies it in
    bool RemoveFile(std::string_view name, std::string& error);  // for good
    bool MoveFile(std::string_view name, std::string_view folder, std::string& error, std::string_view newName = {});
    bool MoveFolder(std::string_view path, std::string_view parent, std::string& error, std::string_view newName = {});
    bool CreateFolder(std::string_view path, std::string& error);
    bool RemoveEmptyFolder(std::string_view path, std::string& error);

    // A file (its path) or a folder moved to `trash` (a folder under .kom/, kept at the same relative path), so it can
    // be restored with what this model knew of it.
    struct Trashed
    {
        std::string path;
        bool folder = false;
        std::vector<ClientDataFile> files;
        std::vector<std::string> folders;
    };
    bool Trash(std::string_view path, bool folder, const std::filesystem::path& trash, Trashed& out,
               std::string& error);
    bool Restore(const Trashed& item, const std::filesystem::path& trash, std::string& error);

    // Writes the hash cache from the files as this model has them (moved files keep their hashes).
    void SaveHashCache() const;

private:
    std::string RecordKey(const ClientDataFile& file) const;
    bool Hash(unsigned jobs, std::string& error, const Progress& progress);
    // States against the manifest of the files on disk; rebuilds the indexes.
    void Classify(std::vector<ClientDataFile> disk);
    std::vector<ClientDataFile> OnDisk() const;  // files_ but the deleted
    void AddFolderChain(const std::string& path);

    std::filesystem::path root_;
    Manifest manifest_;
    std::vector<ClientDataFile> files_;
    std::set<std::string> folders_;
    std::unordered_map<std::string, std::size_t> byName_;  // AsciiLower(name) -> files_
    std::map<std::string, std::string> edits_;             // AsciiLower(record or file name) -> archive
    std::vector<std::string> newArchives_;                 // CreateArchive, until a sync
    IgnoreList ignore_;
    std::unordered_map<std::string, std::vector<std::uint32_t>> folderCounts_;  // folder -> tracked files by archive
};

// Where a client_data's manager keeps file hashes between runs.
std::filesystem::path HashCachePath(const std::filesystem::path& clientData);

} // namespace x2::assets
