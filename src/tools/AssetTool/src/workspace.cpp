#include "x2/assets/workspace.h"

#include "util.h"

#include "x2/assets/client_data.h"
#include "x2/kom/codec.h"

#include <algorithm>
#include <atomic>
#include <charconv>
#include <fstream>
#include <set>
#include <sstream>

namespace x2::assets {

namespace {

using namespace detail;

constexpr std::string_view kHashMagic = "x2-assets-hashes\t1";

struct CachedHash
{
    std::uint64_t size = 0;
    std::int64_t time = 0;
    std::string sha256;
};

std::unordered_map<std::string, CachedHash> ReadHashCache(const std::filesystem::path& path)
{
    std::unordered_map<std::string, CachedHash> cache;
    std::ifstream in(path, std::ios::binary);
    std::string line;
    if (!std::getline(in, line) || line != kHashMagic)
        return cache;  // missing or another version: hash everything again
    while (std::getline(in, line))
    {
        const auto t1 = line.find('\t'), t2 = line.find('\t', t1 + 1), t3 = line.find('\t', t2 + 1);
        if (t3 == std::string::npos)
            continue;
        CachedHash h;
        const char* p = line.data();
        if (std::from_chars(p + t1 + 1, p + t2, h.size).ec != std::errc() ||
            std::from_chars(p + t2 + 1, p + t3, h.time).ec != std::errc())
            continue;
        h.sha256 = line.substr(t3 + 1);
        cache.emplace(line.substr(0, t1), std::move(h));
    }
    return cache;
}

std::string Relative(const std::filesystem::path& root, const std::filesystem::path& path)
{
    return PathToUtf8(path.lexically_relative(root).generic_u8string());
}

// A folder given by a user or a tool: relative, '/', inside client_data and outside its .kom/.
bool CleanFolder(std::string_view folder, std::string& clean, std::string& error)
{
    clean.clear();
    std::size_t start = 0;
    while (start <= folder.size())
    {
        std::size_t end = folder.find_first_of("/\\", start);
        if (end == std::string_view::npos)
            end = folder.size();
        const std::string_view part = folder.substr(start, end - start);
        if (part == ".." || (clean.empty() && part == kMetaFolder) || part.find(':') != std::string_view::npos ||
            (start == 0 && end == 0 && !folder.empty()))
        {
            error = "\"" + std::string(folder) + "\" is not a folder inside client_data";
            return false;
        }
        if (!part.empty() && part != ".")
            clean += (clean.empty() ? "" : "/") + std::string(part);
        start = end + 1;
    }
    return true;
}

bool UnderFolder(const std::string& folder, std::string_view root)
{
    return root.empty() || folder == root ||
           (folder.size() > root.size() && folder.compare(0, root.size(), root) == 0 && folder[root.size()] == '/');
}

} // namespace

std::string_view ToString(FileState state)
{
    switch (state)
    {
    case FileState::Unchanged: return "unchanged";
    case FileState::Modified: return "modified";
    case FileState::Deleted: return "deleted";
    case FileState::Untracked: return "untracked";
    case FileState::Added: return "added";
    }
    return "?";
}

std::filesystem::path HashCachePath(const std::filesystem::path& clientData)
{
    return clientData / kMetaFolder / "hashes.tsv";
}

bool ClientData::Open(const std::filesystem::path& root, unsigned jobs, std::string& error, const Progress& progress)
{
    root_ = root;
    edits_.clear();
    if (!manifest_.Read(ManifestPath(root_), error))
        return false;
    return Rescan(jobs, error, progress);
}

bool ClientData::Rescan(unsigned jobs, std::string& error, const Progress& progress)
{
    return Hash(jobs, error, progress);
}

bool ClientData::Hash(unsigned jobs, std::string& error, const Progress& progress)
{
    std::vector<std::filesystem::path> paths;
    if (!IndexFiles(root_, paths, error))
        return false;

    // Hash what changed since the last run.
    const auto cache = ReadHashCache(HashCachePath(root_));
    std::vector<ClientDataFile> disk(paths.size());
    std::vector<std::int64_t> times(paths.size());
    std::atomic<std::size_t> done{0};
    if (!ParallelFor(paths.size(), jobs, [&](std::size_t i, std::string& e) {
            ClientDataFile& f = disk[i];
            const std::string relative = Relative(root_, paths[i]);
            f.name = PathToUtf8(paths[i].filename());
            const auto slash = relative.rfind('/');
            f.folder = slash == std::string::npos ? std::string() : relative.substr(0, slash);
            std::error_code ec;
            f.size = std::filesystem::file_size(paths[i], ec);
            times[i] = std::filesystem::last_write_time(paths[i], ec).time_since_epoch().count();
            if (const auto it = cache.find(relative);
                it != cache.end() && it->second.size == f.size && it->second.time == times[i])
            {
                f.sha256 = it->second.sha256;
            }
            else
            {
                Bytes data;
                if (!ReadWholeFile(paths[i], data, e))
                    return false;
                f.sha256 = Sha256Hex(data);
            }
            if (progress)
                progress(++done, paths.size());
            return true;
        }, error))
        return false;

    {
        std::ostringstream out;
        out << kHashMagic << '\n';
        for (std::size_t i = 0; i < disk.size(); ++i)
            out << Relative(root_, paths[i]) << '\t' << disk[i].size << '\t' << times[i] << '\t' << disk[i].sha256
                << '\n';
        const std::string text = out.str();
        std::string ignored;  // only a cache: a read-only client_data hashes again next time
        WriteWholeFile(HashCachePath(root_), std::span(reinterpret_cast<const std::uint8_t*>(text.data()), text.size()),
                       ignored);
    }

    // States against the manifest.
    std::unordered_map<std::string, std::size_t> onDisk;
    for (std::size_t i = 0; i < disk.size(); ++i)
        onDisk.emplace(kom::AsciiLower(disk[i].name), i);
    std::vector<ClientDataFile> deleted;
    for (std::size_t k = 0; k < manifest_.files.size(); ++k)
    {
        const FileRecord& r = manifest_.files[k];
        std::vector<std::size_t> present;
        std::vector<std::string> hashes;
        bool missing = false;
        for (const std::string& name : ContentFiles(r))
        {
            if (const auto it = onDisk.find(kom::AsciiLower(name)); it != onDisk.end())
            {
                present.push_back(it->second);
                hashes.push_back(disk[it->second].sha256);
            }
            else
            {
                missing = true;
                ClientDataFile f;
                f.name = name;
                f.state = FileState::Deleted;
                f.record = static_cast<std::ptrdiff_t>(k);
                deleted.push_back(std::move(f));
            }
        }
        FileState state = FileState::Modified;
        if (r.encoding == Encoding::New)
            state = FileState::Added;
        else if (!missing)
        {
            const std::string hash = r.form == Form::Kim ? KimContentHash(hashes[0], hashes[1]) : hashes[0];
            if (hash == r.sha256)
                state = FileState::Unchanged;
        }
        for (const std::size_t i : present)
        {
            disk[i].record = static_cast<std::ptrdiff_t>(k);
            disk[i].state = state;
        }
    }
    for (ClientDataFile& f : disk)
        if (f.record < 0)
            f.state = FileState::Untracked;

    std::sort(disk.begin(), disk.end(), [](const ClientDataFile& a, const ClientDataFile& b) {
        const std::string fa = kom::AsciiLower(a.folder), fb = kom::AsciiLower(b.folder);
        return fa != fb ? fa < fb : kom::AsciiLower(a.name) < kom::AsciiLower(b.name);
    });
    files_ = std::move(disk);
    for (ClientDataFile& f : deleted)
        files_.push_back(std::move(f));
    byName_.clear();
    for (std::size_t i = 0; i < files_.size(); ++i)
        byName_.emplace(kom::AsciiLower(files_[i].name), i);

    // Edits of files that are gone or deleted go.
    std::set<std::string> keys;
    for (const ClientDataFile& f : files_)
        if (f.state != FileState::Deleted)
            keys.insert(RecordKey(f));
    std::erase_if(edits_, [&](const auto& edit) { return !keys.contains(edit.first); });
    return true;
}

const ClientDataFile* ClientData::Find(std::string_view name) const
{
    const auto it = byName_.find(kom::AsciiLower(name));
    return it == byName_.end() ? nullptr : &files_[it->second];
}

std::filesystem::path ClientData::Locate(std::string_view name) const
{
    const ClientDataFile* f = Find(name);
    if (!f || f->state == FileState::Deleted)
        return {};
    return (f->folder.empty() ? root_ : root_ / PathFromUtf8(f->folder)) / PathFromUtf8(f->name);
}

std::string ClientData::FolderForNew(std::string_view name, std::span<const std::string> siblings) const
{
    if (const ClientDataFile* f = Find(name); f && f->state != FileState::Deleted)
        return f->folder;
    for (const std::string& sibling : siblings)
        if (const ClientDataFile* f = Find(sibling); f && f->state != FileState::Deleted)
            return f->folder;
    return {};
}

std::string ClientData::DefaultArchive(std::string_view folder) const
{
    std::unordered_map<std::string, std::size_t> order;
    for (std::size_t a = 0; a < manifest_.archives.size(); ++a)
        order.emplace(manifest_.archives[a].name, a);
    std::string at(folder);
    for (;;)
    {
        std::vector<std::size_t> counts(manifest_.archives.size(), 0);
        bool any = false;
        for (const ClientDataFile& f : files_)
            if (f.record >= 0 && f.state != FileState::Deleted && UnderFolder(f.folder, at))
                if (const auto it = order.find(manifest_.files[static_cast<std::size_t>(f.record)].archive);
                    it != order.end())
                {
                    ++counts[it->second];
                    any = true;
                }
        if (any)
        {
            const auto best = std::max_element(counts.begin(), counts.end());  // first of the largest
            return manifest_.archives[static_cast<std::size_t>(best - counts.begin())].name;
        }
        if (at.empty())
            return {};
        const auto slash = at.rfind('/');
        at = slash == std::string::npos ? std::string() : at.substr(0, slash);
    }
}

std::string ClientData::RecordKey(const ClientDataFile& file) const
{
    return kom::AsciiLower(file.record >= 0 ? manifest_.files[static_cast<std::size_t>(file.record)].name : file.name);
}

std::string ClientData::ArchiveOf(const ClientDataFile& file) const
{
    if (const auto it = edits_.find(RecordKey(file)); it != edits_.end())
        return it->second;
    if (file.record >= 0)
        return manifest_.files[static_cast<std::size_t>(file.record)].archive;
    return DefaultArchive(file.folder);
}

bool ClientData::HasArchiveEdit(const ClientDataFile& file) const
{
    return edits_.contains(RecordKey(file));
}

bool ClientData::SetArchive(std::string_view name, std::string_view archive, std::string& error)
{
    const ClientDataFile* f = Find(name);
    if (!f)
    {
        error = "\"" + std::string(name) + "\" is not in client_data";
        return false;
    }
    if (f->state == FileState::Deleted)
    {
        error = "\"" + f->name + "\" is deleted: a sync drops it";
        return false;
    }
    const auto known = std::find_if(manifest_.archives.begin(), manifest_.archives.end(), [&](const ArchiveRecord& a) {
        return kom::AsciiLower(a.name) == kom::AsciiLower(archive);
    });
    if (known == manifest_.archives.end())
    {
        error = "\"" + std::string(archive) + "\" is not an archive of the manifest";
        return false;
    }
    const std::string key = RecordKey(*f);
    const std::string base =
        f->record >= 0 ? manifest_.files[static_cast<std::size_t>(f->record)].archive : DefaultArchive(f->folder);
    if (base == known->name)
        edits_.erase(key);
    else
        edits_[key] = known->name;
    return true;
}

bool ClientData::SetFolderArchive(std::string_view folder, std::string_view archive, std::size_t& count,
                                  std::string& error)
{
    count = 0;
    std::string clean;
    if (!CleanFolder(folder, clean, error))
        return false;
    // Names first: SetArchive may change what DefaultArchive gives for the rest, but not which files are listed.
    std::vector<std::string> names;
    for (const ClientDataFile& f : files_)
        if (f.state != FileState::Deleted && UnderFolder(f.folder, clean))
            names.push_back(f.name);
    for (const std::string& name : names)
    {
        const std::string before = ArchiveOf(*Find(name));
        if (!SetArchive(name, archive, error))
            return false;
        if (ArchiveOf(*Find(name)) != before)
            ++count;
    }
    return true;
}

bool ClientData::Sync(unsigned jobs, SyncStats& stats, std::string& error)
{
    stats = {};
    std::vector<FileRecord> records = manifest_.files;
    std::unordered_map<std::string, std::uint32_t> next;  // archive -> next free index
    std::unordered_map<std::string, std::map<std::uint32_t, std::size_t>> algorithms;
    for (const FileRecord& r : records)
    {
        auto& n = next[r.archive];
        n = std::max(n, r.index + 1);
        ++algorithms[r.archive][r.algorithm];
    }
    const auto mostCommonAlgorithm = [&](const std::string& archive) -> std::uint32_t {
        const auto it = algorithms.find(archive);
        if (it == algorithms.end())
            return 0;
        return std::max_element(it->second.begin(), it->second.end(), [](const auto& a, const auto& b) {
                   return a.second < b.second;
               })->first;
    };

    // Tracked files first (deleted, moved), then the new ones, so indices follow that order.
    std::vector<bool> drop(records.size(), false), moved(records.size(), false);
    for (const ClientDataFile& f : files_)
    {
        if (f.record < 0)
            continue;
        const auto k = static_cast<std::size_t>(f.record);
        if (f.state == FileState::Deleted)
        {
            drop[k] = true;
            continue;
        }
        const std::string archive = ArchiveOf(f);
        if (!moved[k] && records[k].archive != archive)
        {
            records[k].archive = archive;
            records[k].index = next[archive]++;
            moved[k] = true;
        }
    }
    std::vector<FileRecord> added;
    for (const ClientDataFile& f : files_)
    {
        if (f.record >= 0)
            continue;
        const std::string archive = ArchiveOf(f);
        if (archive.empty())
        {
            error = f.name + ": no archive to add it to (nothing in client_data is tracked)";
            return false;
        }
        FileRecord r;
        r.name = f.name;
        r.archive = archive;
        r.index = next[archive]++;
        r.algorithm = mostCommonAlgorithm(archive);
        r.encoding = Encoding::New;
        r.form = Form::Raw;
        r.sha256 = f.sha256;
        added.push_back(std::move(r));
    }

    Manifest manifest;
    manifest.archives = manifest_.archives;
    for (std::size_t k = 0; k < records.size(); ++k)
    {
        if (drop[k])
            ++stats.removed;
        else
        {
            stats.moved += moved[k];
            manifest.files.push_back(std::move(records[k]));
        }
    }
    stats.added = added.size();
    for (FileRecord& r : added)
        manifest.files.push_back(std::move(r));

    // Archive order, then index, as import writes it; counts of what each archive holds now.
    std::unordered_map<std::string, std::size_t> order;
    for (std::size_t a = 0; a < manifest.archives.size(); ++a)
        order.emplace(manifest.archives[a].name, a);
    std::stable_sort(manifest.files.begin(), manifest.files.end(), [&](const FileRecord& a, const FileRecord& b) {
        const std::size_t oa = order.at(a.archive), ob = order.at(b.archive);
        return oa != ob ? oa < ob : a.index < b.index;
    });
    for (ArchiveRecord& a : manifest.archives)
        a.count = static_cast<std::uint32_t>(std::count_if(manifest.files.begin(), manifest.files.end(),
                                                           [&](const FileRecord& r) { return r.archive == a.name; }));

    if (!manifest.Write(ManifestPath(root_), error))
        return false;
    manifest_ = std::move(manifest);
    edits_.clear();
    return Rescan(jobs, error);
}

bool ClientData::AddFile(const std::filesystem::path& source, std::string_view folder, std::string& error)
{
    std::string clean;
    if (!CleanFolder(folder, clean, error))
        return false;
    const std::string name = PathToUtf8(source.filename());
    if (const ClientDataFile* f = Find(name); f && f->state != FileState::Deleted)
    {
        error = "\"" + name + "\" is already in client_data, in " + (f->folder.empty() ? "its root" : f->folder) +
                " (client_data names files, not paths)";
        return false;
    }
    const std::filesystem::path dir = clean.empty() ? root_ : root_ / PathFromUtf8(clean);
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    if (!std::filesystem::copy_file(source, dir / source.filename(), ec))
    {
        error = "cannot copy " + PathToUtf8(source) + " to " + PathToUtf8(dir) + ": " + ec.message();
        return false;
    }
    return true;
}

bool ClientData::RemoveFile(std::string_view name, std::string& error)
{
    const std::filesystem::path path = Locate(name);
    std::error_code ec;
    if (path.empty() || !std::filesystem::remove(path, ec))
    {
        error = "cannot remove \"" + std::string(name) + "\"" + (ec ? ": " + ec.message() : ": not in client_data");
        return false;
    }
    return true;
}

bool ClientData::MoveFile(std::string_view name, std::string_view folder, std::string& error)
{
    std::string clean;
    if (!CleanFolder(folder, clean, error))
        return false;
    const std::filesystem::path from = Locate(name);
    if (from.empty())
    {
        error = "\"" + std::string(name) + "\" is not in client_data";
        return false;
    }
    const std::filesystem::path dir = clean.empty() ? root_ : root_ / PathFromUtf8(clean);
    const std::filesystem::path to = dir / from.filename();
    std::error_code ec;
    if (std::filesystem::exists(to, ec))
    {
        error = PathToUtf8(to) + " already exists";
        return false;
    }
    std::filesystem::create_directories(dir, ec);
    std::filesystem::rename(from, to, ec);
    if (ec)
    {
        error = "cannot move " + PathToUtf8(from) + " to " + PathToUtf8(dir) + ": " + ec.message();
        return false;
    }
    return true;
}

} // namespace x2::assets
