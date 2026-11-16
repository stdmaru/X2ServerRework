#include "x2/assets/workspace.h"

#include "util.h"

#include "x2/assets/client_data.h"
#include "x2/kom/codec.h"

#include <algorithm>
#include <atomic>
#include <charconv>
#include <fstream>
#include <optional>
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
    case FileState::Ignored: return "ignored";
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
    std::vector<std::filesystem::path> paths, dirs;
    if (!IndexFiles(root_, paths, error, &dirs) || !ignore_.Load(root_, error))
        return false;
    folders_.clear();
    for (const auto& dir : dirs)
        folders_.insert(Relative(root_, dir));

    // Hash what changed: what this model knows first (moved files keep their hashes), then the cache file.
    auto cache = ReadHashCache(HashCachePath(root_));
    for (const ClientDataFile& f : files_)
        if (f.state != FileState::Deleted)
            cache[f.folder.empty() ? f.name : f.folder + "/" + f.name] = CachedHash{f.size, f.time, f.sha256};
    std::vector<ClientDataFile> disk(paths.size());
    std::atomic<std::size_t> done{0};
    if (!ParallelFor(paths.size(), jobs, [&](std::size_t i, std::string& e) {
            ClientDataFile& f = disk[i];
            const std::string relative = Relative(root_, paths[i]);
            f.name = PathToUtf8(paths[i].filename());
            const auto slash = relative.rfind('/');
            f.folder = slash == std::string::npos ? std::string() : relative.substr(0, slash);
            std::error_code ec;
            f.size = std::filesystem::file_size(paths[i], ec);
            f.time = std::filesystem::last_write_time(paths[i], ec).time_since_epoch().count();
            if (const auto it = cache.find(relative);
                it != cache.end() && it->second.size == f.size && it->second.time == f.time)
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

    // By folder, then name, case-insensitive (keys made once: lower-casing in the comparison took seconds).
    std::vector<std::pair<std::string, std::size_t>> order(disk.size());
    for (std::size_t i = 0; i < disk.size(); ++i)
        order[i] = {kom::AsciiLower(disk[i].folder) + '\0' + kom::AsciiLower(disk[i].name), i};
    std::sort(order.begin(), order.end());
    std::vector<ClientDataFile> sorted;
    sorted.reserve(disk.size());
    for (const auto& [key, i] : order)
        sorted.push_back(std::move(disk[i]));
    Classify(std::move(sorted));
    SaveHashCache();
    return true;
}

void ClientData::SaveHashCache() const
{
    std::ostringstream out;
    out << kHashMagic << '\n';
    for (const ClientDataFile& f : files_)
        if (f.state != FileState::Deleted)
            out << (f.folder.empty() ? f.name : f.folder + "/" + f.name) << '\t' << f.size << '\t' << f.time << '\t'
                << f.sha256 << '\n';
    const std::string text = out.str();
    std::string ignored;  // only a cache: a read-only client_data hashes again next time
    WriteWholeFile(HashCachePath(root_), std::span(reinterpret_cast<const std::uint8_t*>(text.data()), text.size()),
                   ignored);
}

std::vector<ClientDataFile> ClientData::OnDisk() const
{
    std::vector<ClientDataFile> disk;
    disk.reserve(files_.size());
    for (const ClientDataFile& f : files_)
        if (f.state != FileState::Deleted)
            disk.push_back(f);
    return disk;
}

void ClientData::AddFolderChain(const std::string& path)
{
    std::string at = path;
    while (!at.empty())
    {
        folders_.insert(at);
        const auto slash = at.rfind('/');
        at = slash == std::string::npos ? std::string() : at.substr(0, slash);
    }
}

void ClientData::Classify(std::vector<ClientDataFile> disk)
{
    std::unordered_map<std::string, std::size_t> onDisk;
    for (std::size_t i = 0; i < disk.size(); ++i)
    {
        disk[i].record = -1;
        onDisk.emplace(kom::AsciiLower(disk[i].name), i);
    }
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
            f.state = ignore_.Matches(f.folder.empty() ? f.name : f.folder + "/" + f.name) ? FileState::Ignored
                                                                                         : FileState::Untracked;

    files_ = std::move(disk);
    for (ClientDataFile& f : deleted)
        files_.push_back(std::move(f));
    byName_.clear();
    for (std::size_t i = 0; i < files_.size(); ++i)
        byName_.emplace(kom::AsciiLower(files_[i].name), i);
    for (const ClientDataFile& f : files_)
        if (f.state != FileState::Deleted)
            AddFolderChain(f.folder);

    // Tracked files by archive under every folder (its own and its subfolders'), for DefaultArchive.
    folderCounts_.clear();
    std::unordered_map<std::string, std::size_t> order;
    for (std::size_t a = 0; a < manifest_.archives.size(); ++a)
        order.emplace(manifest_.archives[a].name, a);
    for (const ClientDataFile& f : files_)
    {
        if (f.record < 0 || f.state == FileState::Deleted)
            continue;
        const auto a = order.find(manifest_.files[static_cast<std::size_t>(f.record)].archive);
        if (a == order.end())
            continue;
        std::string at = f.folder;
        for (;;)
        {
            auto& counts = folderCounts_[at];
            counts.resize(manifest_.archives.size(), 0);
            ++counts[a->second];
            if (at.empty())
                break;
            const auto slash = at.rfind('/');
            at = slash == std::string::npos ? std::string() : at.substr(0, slash);
        }
    }

    // Edits of files that are gone, deleted or ignored go.
    std::set<std::string> keys;
    for (const ClientDataFile& f : files_)
        if (f.state != FileState::Deleted && f.state != FileState::Ignored)
            keys.insert(RecordKey(f));
    std::erase_if(edits_, [&](const auto& edit) { return !keys.contains(edit.first); });
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
    std::string at(folder);
    for (;;)
    {
        if (const auto it = folderCounts_.find(at); it != folderCounts_.end())
        {
            const auto best = std::max_element(it->second.begin(), it->second.end());  // first of the largest
            return manifest_.archives[static_cast<std::size_t>(best - it->second.begin())].name;
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
    if (f->state == FileState::Deleted || f->state == FileState::Ignored)
    {
        error = "\"" + f->name + "\" is " + std::string(ToString(f->state)) + ": it goes to no archive";
        return false;
    }
    const std::vector<std::string> archives = Archives();
    const auto known = std::find_if(archives.begin(), archives.end(), [&](const std::string& a) {
        return kom::AsciiLower(a) == kom::AsciiLower(archive);
    });
    if (known == archives.end())
    {
        error = "\"" + std::string(archive) + "\" is not an archive of the manifest (create it first)";
        return false;
    }
    const std::string key = RecordKey(*f);
    const std::string base =
        f->record >= 0 ? manifest_.files[static_cast<std::size_t>(f->record)].archive : DefaultArchive(f->folder);
    if (base == *known)
        edits_.erase(key);
    else
        edits_[key] = *known;
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
        if (f.state != FileState::Deleted && f.state != FileState::Ignored && UnderFolder(f.folder, clean))
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

bool ClientData::Sync(SyncStats& stats, std::string& error)
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
        if (f.record >= 0 || f.state == FileState::Ignored)
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
    for (const std::string& name : newArchives_)  // in the client's load order: names sorted (ArchivesInLoadOrder)
    {
        const auto at = std::find_if(manifest.archives.begin(), manifest.archives.end(), [&](const ArchiveRecord& a) {
            return kom::AsciiLower(a.name) > kom::AsciiLower(name);
        });
        manifest.archives.insert(at, ArchiveRecord{name, 0});
        ++stats.archives;
    }
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

    std::error_code ec;  // the manifest before this sync, in case it was not what the user meant
    std::filesystem::copy_file(ManifestPath(root_), ManifestPath(root_).string() + ".bak",
                               std::filesystem::copy_options::overwrite_existing, ec);
    if (!manifest.Write(ManifestPath(root_), error))
        return false;
    manifest_ = std::move(manifest);
    DiscardEdits();
    Classify(OnDisk());
    return true;
}

std::vector<std::string> ClientData::Archives() const
{
    std::vector<std::string> names;
    for (const ArchiveRecord& a : manifest_.archives)
        names.push_back(a.name);
    names.insert(names.end(), newArchives_.begin(), newArchives_.end());
    return names;
}

bool ClientData::CreateArchive(std::string_view name, std::string& error)
{
    const std::string lower = kom::AsciiLower(name);
    if (lower.size() <= 4 || !lower.ends_with(".kom") || lower.find_first_of("/\\:*?\"<>|\t") != std::string::npos)
    {
        error = "\"" + std::string(name) + "\" is not an archive name (a file name ending in .kom)";
        return false;
    }
    for (const std::string& a : Archives())
        if (kom::AsciiLower(a) == lower)
        {
            error = a + " already exists";
            return false;
        }
    newArchives_.emplace_back(name);
    return true;
}

bool ClientData::Ignore(std::string_view name, std::string& error)
{
    const ClientDataFile* f = Find(name);
    if (!f)
        return IgnorePattern(name, error);  // not there yet: its name, wherever it appears
    if (f->state != FileState::Untracked)
    {
        error = f->name + (f->state == FileState::Ignored ? " is already ignored"
                                                          : " is in the manifest: only files it does not hold can be "
                                                            "ignored (remove it first)");
        return false;
    }
    return IgnorePattern(f->folder.empty() ? f->name : f->folder + "/" + f->name, error);
}

bool ClientData::IgnorePattern(std::string_view pattern, std::string& error)
{
    if (pattern.empty() || pattern == "/" || pattern.find_first_of("\t\n") != std::string_view::npos)
    {
        error = "\"" + std::string(pattern) + "\" is not an ignore pattern";
        return false;
    }
    ignore_.patterns.emplace_back(pattern);
    if (!ignore_.Save(root_, error))
        return false;
    Classify(OnDisk());
    return true;
}

bool ClientData::IgnoreFolder(std::string_view folder, std::string& error)
{
    std::string clean;
    if (!CleanFolder(folder, clean, error))
        return false;
    if (clean.empty())
    {
        error = "client_data's root cannot be ignored";
        return false;
    }
    ignore_.patterns.push_back(clean + "/");
    if (!ignore_.Save(root_, error))
        return false;
    Classify(OnDisk());
    return true;
}

bool ClientData::Unignore(std::string_view name, std::string& error)
{
    const ClientDataFile* f = Find(name);
    if (!f || f->state != FileState::Ignored)
    {
        error = "\"" + std::string(name) + "\" is not ignored";
        return false;
    }
    const std::string path = f->folder.empty() ? f->name : f->folder + "/" + f->name;
    std::vector<std::string> kept;
    for (const std::string& p : ignore_.patterns)
    {
        IgnoreList one;
        one.patterns = {p};
        if (!one.Matches(path))
            kept.push_back(p);
        else if (kom::AsciiLower(p) != kom::AsciiLower(path))
        {
            error = "ignored by the pattern \"" + p + "\" in " + PathToUtf8(IgnorePath(root_)) +
                    ": change it there, or stop ignoring the folder";
            return false;
        }
    }
    ignore_.patterns = std::move(kept);
    if (!ignore_.Save(root_, error))
        return false;
    Classify(OnDisk());
    return true;
}

bool ClientData::SetIgnores(IgnoreList list, std::string& error)
{
    ignore_ = std::move(list);
    if (!ignore_.Save(root_, error))
        return false;
    Classify(OnDisk());
    return true;
}

namespace {

std::string Join(const std::string& folder, std::string_view name)
{
    return folder.empty() ? std::string(name) : folder + "/" + std::string(name);
}

bool GoodName(std::string_view name, std::string& error)
{
    if (name.empty() || name == "." || name == ".." || name.find_first_of("/\\:") != std::string_view::npos)
    {
        error = "\"" + std::string(name) + "\" is not a file or folder name";
        return false;
    }
    return true;
}

} // namespace

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
    const std::filesystem::path target = dir / source.filename();
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    if (!std::filesystem::copy_file(source, target, ec))
    {
        error = "cannot copy " + PathToUtf8(source) + " to " + PathToUtf8(dir) + ": " + ec.message();
        return false;
    }
    ClientDataFile f;
    f.name = name;
    f.folder = clean;
    Bytes data;
    if (!ReadWholeFile(target, data, error))
        return false;
    f.size = data.size();
    f.time = std::filesystem::last_write_time(target, ec).time_since_epoch().count();
    f.sha256 = Sha256Hex(data);
    auto disk = OnDisk();
    disk.push_back(std::move(f));
    Classify(std::move(disk));
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
    auto disk = OnDisk();
    std::erase_if(disk, [&](const ClientDataFile& f) { return kom::AsciiLower(f.name) == kom::AsciiLower(name); });
    Classify(std::move(disk));
    return true;
}

bool ClientData::MoveFile(std::string_view name, std::string_view folder, std::string& error, std::string_view newName)
{
    std::string clean;
    if (!CleanFolder(folder, clean, error))
        return false;
    const ClientDataFile* f = Find(name);
    if (!f || f->state == FileState::Deleted)
    {
        error = "\"" + std::string(name) + "\" is not in client_data";
        return false;
    }
    const std::string oldName = f->name, oldFolder = f->folder;
    const std::string to = newName.empty() ? oldName : std::string(newName);
    if (!GoodName(to, error))
        return false;
    if (kom::AsciiLower(to) != kom::AsciiLower(oldName))
        if (const ClientDataFile* other = Find(to); other && other->state != FileState::Deleted)
        {
            error = "\"" + to + "\" is already in client_data, in " + (other->folder.empty() ? "its root" : other->folder);
            return false;
        }
    if (clean == oldFolder && to == oldName)
        return true;
    const std::filesystem::path dir = clean.empty() ? root_ : root_ / PathFromUtf8(clean);
    const std::filesystem::path from = Locate(oldName), target = dir / PathFromUtf8(to);
    std::error_code ec;
    if (std::filesystem::exists(target, ec) && !std::filesystem::equivalent(from, target, ec))
    {
        error = PathToUtf8(target) + " already exists";
        return false;
    }
    std::filesystem::create_directories(dir, ec);
    std::filesystem::rename(from, target, ec);
    if (ec)
    {
        error = "cannot move " + PathToUtf8(from) + " to " + PathToUtf8(dir) + ": " + ec.message();
        return false;
    }
    auto disk = OnDisk();
    for (ClientDataFile& d : disk)
        if (kom::AsciiLower(d.name) == kom::AsciiLower(oldName))
        {
            d.name = to;
            d.folder = clean;
        }
    AddFolderChain(clean);
    Classify(std::move(disk));
    return true;
}

bool ClientData::MoveFolder(std::string_view path, std::string_view parent, std::string& error, std::string_view newName)
{
    std::string from, into;
    if (!CleanFolder(path, from, error) || !CleanFolder(parent, into, error))
        return false;
    if (from.empty() || !folders_.contains(from))
    {
        error = "\"" + std::string(path) + "\" is not a folder of client_data";
        return false;
    }
    const std::string last = from.substr(from.rfind('/') == std::string::npos ? 0 : from.rfind('/') + 1);
    const std::string name = newName.empty() ? last : std::string(newName);
    if (!GoodName(name, error))
        return false;
    const std::string to = Join(into, name);
    if (to == from)
        return true;
    if (to.starts_with(from + "/"))
    {
        error = "a folder cannot go into itself";
        return false;
    }
    std::error_code ec;
    if (std::filesystem::exists(root_ / PathFromUtf8(to), ec))
    {
        error = to + " already exists";
        return false;
    }
    if (!into.empty())
        std::filesystem::create_directories(root_ / PathFromUtf8(into), ec);
    std::filesystem::rename(root_ / PathFromUtf8(from), root_ / PathFromUtf8(to), ec);
    if (ec)
    {
        error = "cannot move " + from + " to " + to + ": " + ec.message();
        return false;
    }
    const auto moved = [&](const std::string& folder) -> std::optional<std::string> {
        if (folder == from)
            return to;
        if (folder.starts_with(from + "/"))
            return to + folder.substr(from.size());
        return std::nullopt;
    };
    auto disk = OnDisk();
    for (ClientDataFile& d : disk)
        if (const auto m = moved(d.folder))
            d.folder = *m;
    std::set<std::string> folders;
    for (const std::string& folder : folders_)
        folders.insert(moved(folder).value_or(folder));
    folders_ = std::move(folders);
    AddFolderChain(to);
    Classify(std::move(disk));
    return true;
}

bool ClientData::CreateFolder(std::string_view path, std::string& error)
{
    std::string clean;
    if (!CleanFolder(path, clean, error))
        return false;
    std::error_code ec;
    if (clean.empty() || std::filesystem::exists(root_ / PathFromUtf8(clean), ec))
    {
        error = (clean.empty() ? std::string("the root") : clean) + " already exists";
        return false;
    }
    if (!std::filesystem::create_directories(root_ / PathFromUtf8(clean), ec))
    {
        error = "cannot create " + clean + ": " + ec.message();
        return false;
    }
    AddFolderChain(clean);
    return true;
}

bool ClientData::RemoveEmptyFolder(std::string_view path, std::string& error)
{
    std::string clean;
    if (!CleanFolder(path, clean, error))
        return false;
    std::error_code ec;
    if (clean.empty() || !std::filesystem::remove(root_ / PathFromUtf8(clean), ec))
    {
        error = "cannot remove " + clean + (ec ? ": " + ec.message() : std::string());
        return false;
    }
    folders_.erase(clean);
    return true;
}

bool ClientData::Trash(std::string_view path, bool folder, const std::filesystem::path& trash, Trashed& out,
                       std::string& error)
{
    out = {};
    if (!CleanFolder(path, out.path, error))
        return false;
    out.folder = folder;
    if (out.path.empty())
    {
        error = "client_data's root cannot be deleted";
        return false;
    }
    const std::filesystem::path source = root_ / PathFromUtf8(out.path), target = trash / PathFromUtf8(out.path);
    std::error_code ec;
    std::filesystem::remove_all(target, ec);
    std::filesystem::create_directories(target.parent_path(), ec);
    std::filesystem::rename(source, target, ec);
    if (ec)
    {
        error = "cannot delete " + out.path + ": " + ec.message();
        return false;
    }
    const auto inside = [&](const ClientDataFile& f) {
        return folder ? f.folder == out.path || f.folder.starts_with(out.path + "/") : Join(f.folder, f.name) == out.path;
    };
    auto disk = OnDisk();
    for (const ClientDataFile& f : disk)
        if (inside(f))
            out.files.push_back(f);
    std::erase_if(disk, inside);
    if (folder)
    {
        for (auto it = folders_.begin(); it != folders_.end();)
        {
            if (*it == out.path || it->starts_with(out.path + "/"))
            {
                out.folders.push_back(*it);
                it = folders_.erase(it);
            }
            else
                ++it;
        }
    }
    Classify(std::move(disk));
    return true;
}

bool ClientData::Restore(const Trashed& item, const std::filesystem::path& trash, std::string& error)
{
    const std::filesystem::path source = trash / PathFromUtf8(item.path), target = root_ / PathFromUtf8(item.path);
    std::error_code ec;
    if (std::filesystem::exists(target, ec))
    {
        error = item.path + " is in client_data again: it cannot be restored over it";
        return false;
    }
    for (const ClientDataFile& f : item.files)
        if (const ClientDataFile* other = Find(f.name); other && other->state != FileState::Deleted)
        {
            error = f.name + " is in client_data again, in " + (other->folder.empty() ? "its root" : other->folder);
            return false;
        }
    std::filesystem::create_directories(target.parent_path(), ec);
    std::filesystem::rename(source, target, ec);
    if (ec)
    {
        error = "cannot restore " + item.path + ": " + ec.message();
        return false;
    }
    auto disk = OnDisk();
    disk.insert(disk.end(), item.files.begin(), item.files.end());
    for (const std::string& folder : item.folders)
        folders_.insert(folder);
    const auto slash = item.path.rfind('/');
    if (slash != std::string::npos)
        AddFolderChain(item.path.substr(0, slash));
    Classify(std::move(disk));
    return true;
}

bool ClientData::Commit(unsigned jobs, CommitStats& stats, std::string& error)
{
    stats = {};
    if (!Sync(stats.sync, error))
        return false;
    PackOptions options;
    options.clientData = root_;
    options.jobs = jobs;
    options.write = false;
    options.commit = true;
    PackStats packed;
    if (!Pack(options, packed, error))
        return false;
    stats.committed = packed.committed;
    if (!manifest_.Read(ManifestPath(root_), error))
        return false;
    Classify(OnDisk());
    return true;
}

} // namespace x2::assets
