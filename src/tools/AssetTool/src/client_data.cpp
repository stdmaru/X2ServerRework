#include "x2/assets/client_data.h"

#include "util.h"

#include "x2/assets/workspace.h"

#include "Crypto/Crypto.h"
#include "x2/kom/mount.h"
#include "x2/kom/writer.h"
#include "x2/lua/bytecode.h"
#include "x2/lua/decompile.h"
#include "x2/lua/kim.h"

#include <algorithm>
#include <atomic>
#include <fstream>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <span>
#include <thread>
#include <unordered_map>

namespace x2::assets {

namespace {

using namespace detail;

bool SameFile(const std::filesystem::path& a, const std::filesystem::path& b)
{
    std::error_code ec;
    return std::filesystem::equivalent(a, b, ec);
}

bool EndsWithNoCase(std::string_view name, std::string_view suffix)
{
    return name.size() >= suffix.size() && kom::AsciiLower(name.substr(name.size() - suffix.size())) == suffix;
}

bool IsLuaName(std::string_view name) { return EndsWithNoCase(name, ".lua"); }
bool IsKimName(std::string_view name) { return EndsWithNoCase(name, ".kim"); }

Bytes ToBytes(std::string_view s) { return Bytes(s.begin(), s.end()); }
std::string ToText(const Bytes& b) { return std::string(b.begin(), b.end()); }

constexpr std::string_view kEnumScript = "enum.lua";

// A decoded entry whose file waits for the Lua pass of import.
struct Deferred
{
    std::size_t archive = 0, record = 0;
    std::filesystem::path target;
    Bytes data;
};

} // namespace

bool Import(const ImportOptions& options, ImportStats& stats, std::string& error)
{
    stats = {};
    Log log(options.log);

    std::error_code ec;
    if (std::filesystem::exists(options.destination, ec) &&
        !std::filesystem::is_empty(options.destination, ec))
    {
        error = PathToUtf8(options.destination) + " is not empty; import into a new folder";
        return false;
    }

    // Open every archive (headers only) and plan where each entry goes before writing anything.
    std::vector<std::filesystem::path> paths;
    if (!kom::ArchivesInLoadOrder(options.source, paths, error))
        return false;
    if (paths.empty())
    {
        error = "no .kom archives in " + PathToUtf8(options.source);
        return false;
    }
    std::vector<std::shared_ptr<kom::Archive>> archives;
    std::unordered_map<std::string, std::string> owner;  // AsciiLower(name) -> archive
    std::vector<std::string> duplicates;
    std::uint32_t total = 0;
    for (const auto& path : paths)
    {
        auto archive = kom::Archive::Open(path, error);
        if (!archive)
            return false;
        const std::string archiveName = PathToUtf8(path.filename());
        for (const kom::Entry& e : archive->Entries())
        {
            const auto [it, inserted] = owner.try_emplace(kom::AsciiLower(e.name), archiveName);
            if (!inserted)
                duplicates.push_back(e.name + " (" + it->second + ", " + archiveName + ")");
        }
        total += static_cast<std::uint32_t>(archive->Entries().size());
        archives.push_back(std::move(archive));
    }
    if (!duplicates.empty())
    {
        error = std::to_string(duplicates.size()) + " name(s) in more than one archive, which a manifest keyed by name "
                "cannot hold; first: " + duplicates.front();
        return false;
    }
    if (options.layout.Rules().empty())
        log("no layout rules: every file goes to " + std::string(Layout::kFallback));

    const auto meta = options.destination / kMetaFolder;
    for (const auto& folder : {meta / "headers", meta / "blobs", meta / "originals"})
        if (!std::filesystem::create_directories(folder, ec) && ec)
        {
            error = "cannot create " + PathToUtf8(folder) + ": " + ec.message();
            return false;
        }

    // Folders, created up front so workers never race on create_directories.
    std::vector<std::vector<std::filesystem::path>> targets(archives.size());
    std::map<std::string, bool> folders;
    for (std::size_t a = 0; a < archives.size(); ++a)
    {
        const std::string archiveName = PathToUtf8(archives[a]->Path().filename());
        for (const kom::Entry& e : archives[a]->Entries())
        {
            const std::string folder = options.layout.Classify(e.name, archiveName);
            folders[folder] = true;
            targets[a].push_back(options.destination / PathFromUtf8(folder) / PathFromUtf8(e.name));
        }
    }
    for (const auto& [folder, unused] : folders)
    {
        const auto path = options.destination / PathFromUtf8(folder);
        if (!std::filesystem::create_directories(path, ec) && ec)
        {
            error = "cannot create " + PathToUtf8(path) + ": " + ec.message();
            return false;
        }
    }

    std::vector<std::vector<FileRecord>> records(archives.size());
    std::vector<std::vector<Deferred>> lua(archives.size()), kim(archives.size());
    std::atomic<std::size_t> done{0}, keptBlobs{0}, stored{0}, luaSource{0}, luaKept{0}, kimCount{0};
    std::atomic<std::uint64_t> bytes{0};
    const bool ok = ParallelFor(archives.size(), options.jobs, [&](std::size_t a, std::string& e) {
        const kom::Archive& archive = *archives[a];
        const std::string archiveName = PathToUtf8(archive.Path().filename());
        const auto header = archive.RawHeader();
        if (!WriteWholeFile(HeaderPath(options.destination, archiveName), header, e))
            return false;

        Bytes blob, data, again;
        for (std::size_t i = 0; i < archive.Entries().size(); ++i)
        {
            const kom::Entry& entry = archive.Entries()[i];
            FileRecord r;
            r.name = entry.name;
            r.archive = archiveName;
            r.index = static_cast<std::uint32_t>(i);
            r.algorithm = entry.algorithm;
            r.size = entry.size;
            r.storedSize = entry.storedSize;
            r.checksum = entry.checksum;
            r.fileTime = entry.fileTime;

            if (!archive.ReadStored(entry, blob, e))
                return false;
            const Bytes* content = &blob;
            if (kom::IsDecodable(entry.algorithm))
            {
                if (!kom::Decode(entry.name, entry.algorithm, blob, entry.size, data, e))
                {
                    e = archiveName + ": " + entry.name + ": " + e;
                    return false;
                }
                content = &data;
                if (kom::Encode(entry.name, entry.algorithm, data, again, e, kom::kKogLevel) && again == blob)
                {
                    r.encoding = Encoding::Zlib;
                }
                else
                {
                    r.encoding = Encoding::Blob;
                    if (!WriteWholeFile(BlobPath(options.destination, entry.name), blob, e))
                        return false;
                    ++keptBlobs;
                }
            }
            else
            {
                r.encoding = Encoding::Stored;
                ++stored;
            }
            if (options.readableLua && content == &data && IsLuaName(entry.name) && lua::IsBytecode(data))
            {
                lua[a].push_back({a, records[a].size(), targets[a][i], data});
                records[a].push_back(std::move(r));
                continue;
            }
            bool kimScriptsFree = true;
            for (const std::string& script : KimScriptNames(entry.name))
                kimScriptsFree = kimScriptsFree && !owner.count(kom::AsciiLower(script));
            if (options.readableLua && content == &data && IsKimName(entry.name))
            {
                if (kimScriptsFree)
                {
                    kim[a].push_back({a, records[a].size(), targets[a][i], data});
                    records[a].push_back(std::move(r));
                    continue;
                }
                log(archiveName + ": " + entry.name + ": kept as is (its script names are taken by other entries)");
            }
            r.sha256 = Sha256Hex(*content);
            if (!WriteWholeFile(targets[a][i], *content, e))
                return false;
            bytes += content->size();
            records[a].push_back(std::move(r));
        }
        log("[" + std::to_string(++done) + "/" + std::to_string(archives.size()) + "] " + archiveName + ": " +
            std::to_string(archive.Entries().size()) + " files");
        return true;
    }, error);
    if (!ok)
        return false;

    // Lua: bytecode -> source, kept as bytecode when the decompiler or LuaJIT (reading its output back) fails.
    std::vector<Deferred*> scripts;
    for (auto& list : lua)
        for (Deferred& d : list)
            scripts.push_back(&d);
    if (!scripts.empty())
        log("decompiling " + std::to_string(scripts.size()) + " Lua scripts");
    if (!ParallelFor(scripts.size(), options.jobs, [&](std::size_t k, std::string& e) {
            Deferred& d = *scripts[k];
            FileRecord& r = records[d.archive][d.record];
            std::string source, why;
            Bytes content;
            if (lua::Decompile(d.data, source, why))
            {
                source.insert(0, lua::kBom);
                if (lua::CheckSyntax(source, r.name, why))
                    content = ToBytes(source);
                else
                    why = "LuaJIT cannot read the decompiled source: " + why;
            }
            if (!content.empty())
            {
                if (!WriteWholeFile(OriginalPath(options.destination, r.name), d.data, e))
                    return false;
                r.form = Form::Lua;
                ++luaSource;
            }
            else
            {
                log(r.archive + ": " + r.name + ": kept as bytecode (" + why + ")");
                content = std::move(d.data);
                ++luaKept;
            }
            r.sha256 = Sha256Hex(content);
            if (!WriteWholeFile(d.target, content, e))
                return false;
            bytes += content.size();
            Bytes().swap(d.data);
            return true;
        }, error))
        return false;

    // Item.kim -> Item.lua + SetItem.lua, with the enum names of the (decompiled) Enum.lua.
    for (auto& list : kim)
        for (Deferred& d : list)
        {
            FileRecord& r = records[d.archive][d.record];
            lua::Enums enums;
            std::string why;
            for (std::size_t a = 0; a < records.size() && enums.Empty(); ++a)
                for (std::size_t i = 0; i < records[a].size(); ++i)
                    if (kom::AsciiLower(records[a][i].name) == kEnumScript)
                    {
                        Bytes source;
                        if (!ReadWholeFile(targets[a][i], source, error))
                            return false;
                        if (lua::IsBytecode(source))
                            log(records[a][i].name + ": enum names not available (it is still bytecode)");
                        else if (!lua::LoadEnums({ToText(source)}, enums, why))
                            log(records[a][i].name + ": enum names not available (" + why + ")");
                        break;
                    }
            lua::ItemScripts itemScripts;
            if (lua::DecompileKim(d.data, enums, itemScripts, why))
            {
                if (!WriteWholeFile(OriginalPath(options.destination, r.name), d.data, error))
                    return false;
                const auto names = KimScriptNames(r.name);
                const auto folder = d.target.parent_path();
                const Bytes items = ToBytes(itemScripts.items), sets = ToBytes(itemScripts.setItems);
                if (!WriteWholeFile(folder / PathFromUtf8(names[0]), items, error) ||
                    !WriteWholeFile(folder / PathFromUtf8(names[1]), sets, error))
                    return false;
                r.form = Form::Kim;
                r.sha256 = KimContentHash(Sha256Hex(items), Sha256Hex(sets));
                bytes += items.size() + sets.size();
                ++kimCount;
                log(r.name + ": " + std::to_string(itemScripts.itemCount) + " items, " +
                    std::to_string(itemScripts.setCount) + " sets -> " + names[0] + ", " + names[1] +
                    (enums.Empty() ? " (without enum names)" : ""));
            }
            else
            {
                log(r.archive + ": " + r.name + ": kept as is (" + why + ")");
                r.sha256 = Sha256Hex(d.data);
                if (!WriteWholeFile(d.target, d.data, error))
                    return false;
                bytes += d.data.size();
            }
        }

    Manifest manifest;
    for (std::size_t a = 0; a < archives.size(); ++a)
    {
        manifest.archives.push_back({PathToUtf8(archives[a]->Path().filename()),
                                     static_cast<std::uint32_t>(archives[a]->Entries().size())});
        for (FileRecord& r : records[a])
            manifest.files.push_back(std::move(r));
    }
    if (!manifest.Write(ManifestPath(options.destination), error))
        return false;

    stats.archives = archives.size();
    stats.files = total;
    stats.keptBlobs = keptBlobs;
    stats.stored = stored;
    stats.luaSource = luaSource;
    stats.luaKept = luaKept;
    stats.kim = kimCount;
    stats.bytes = bytes;
    return true;
}

std::filesystem::path IgnorePath(const std::filesystem::path& clientData)
{
    return clientData / kMetaFolder / "ignore.txt";
}

bool IgnoreList::Load(const std::filesystem::path& clientData, std::string& error)
{
    patterns.clear();
    std::error_code ec;
    if (!std::filesystem::exists(IgnorePath(clientData), ec))
        return true;
    std::ifstream in(IgnorePath(clientData), std::ios::binary);
    if (!in)
    {
        error = "cannot open " + PathToUtf8(IgnorePath(clientData));
        return false;
    }
    std::string line;
    while (std::getline(in, line))
    {
        while (!line.empty() && (line.back() == '\r' || line.back() == ' ' || line.back() == '\t'))
            line.pop_back();
        const auto start = line.find_first_not_of(" \t");
        if (start == std::string::npos || line[start] == '#')
            continue;
        patterns.push_back(line.substr(start));
    }
    return true;
}

bool IgnoreList::Save(const std::filesystem::path& clientData, std::string& error) const
{
    std::string text = "# Files x2_assets keeps out of the manifest and the archives (docs/tools/client-data.md#ignored-files)\n";
    for (const std::string& p : patterns)
        text += p + "\n";
    return WriteWholeFile(IgnorePath(clientData), ToBytes(text), error);
}

bool IgnoreList::Matches(std::string_view relative) const
{
    const std::string path = kom::AsciiLower(relative);
    const auto slash = path.rfind('/');
    const std::string_view name = slash == std::string::npos ? std::string_view(path) : std::string_view(path).substr(slash + 1);
    for (const std::string& pattern : patterns)
    {
        const std::string p = kom::AsciiLower(pattern);
        if (p.ends_with('/'))
        {
            if (path.starts_with(p))
                return true;
        }
        else if (p.find('/') != std::string::npos ? GlobMatch(p, path) : GlobMatch(p, name))
        {
            return true;
        }
    }
    return false;
}

bool IndexFiles(const std::filesystem::path& clientData, std::vector<std::filesystem::path>& files,
                std::string& error)
{
    files.clear();
    std::unordered_map<std::string, std::size_t> seen;
    std::error_code ec;
    std::filesystem::recursive_directory_iterator it(clientData, ec), end;
    if (ec)
    {
        error = "cannot list " + PathToUtf8(clientData) + ": " + ec.message();
        return false;
    }
    for (; it != end; it.increment(ec))
    {
        if (ec)
        {
            error = "cannot list " + PathToUtf8(clientData) + ": " + ec.message();
            return false;
        }
        if (it.depth() == 0 && it->is_directory(ec) && it->path().filename() == PathFromUtf8(kMetaFolder))
        {
            it.disable_recursion_pending();
            continue;
        }
        if (!it->is_regular_file(ec))
            continue;
        const std::string key = kom::AsciiLower(PathToUtf8(it->path().filename()));
        const auto [found, inserted] = seen.try_emplace(key, files.size());
        if (!inserted)
        {
            error = "\"" + PathToUtf8(it->path().filename()) + "\" is in two folders: " +
                    PathToUtf8(files[found->second]) + " and " + PathToUtf8(it->path()) +
                    " (client_data names files, not paths)";
            return false;
        }
        files.push_back(it->path());
    }
    return true;
}

namespace {

struct Workspace
{
    Manifest manifest;
    std::vector<std::filesystem::path> files;
    std::unordered_map<std::string, std::size_t> byName;  // AsciiLower(file name) -> files
    std::vector<bool> known;                              // files[i] is in the manifest
    std::vector<bool> ignored;                            // files[i] is untracked and in .kom/ignore.txt

    bool Load(const std::filesystem::path& clientData, std::string& error)
    {
        IgnoreList ignore;
        if (!manifest.Read(ManifestPath(clientData), error) || !IndexFiles(clientData, files, error) ||
            !ignore.Load(clientData, error))
            return false;
        for (std::size_t i = 0; i < files.size(); ++i)
            byName.emplace(kom::AsciiLower(PathToUtf8(files[i].filename())), i);
        known.assign(files.size(), false);
        for (const FileRecord& r : manifest.files)
            for (const std::string& name : ContentFiles(r))
                if (const auto it = byName.find(kom::AsciiLower(name)); it != byName.end())
                    known[it->second] = true;
        ignored.assign(files.size(), false);
        for (std::size_t i = 0; i < files.size(); ++i)
            if (!known[i])
                ignored[i] = ignore.Matches(PathToUtf8(files[i].lexically_relative(clientData).generic_u8string()));
        return true;
    }

    const std::filesystem::path* Find(std::string_view name) const
    {
        const auto it = byName.find(kom::AsciiLower(name));
        return it == byName.end() ? nullptr : &files[it->second];
    }

    // The record's files in client_data and its content hash; false if one is missing.
    bool Content(const FileRecord& r, std::vector<Bytes>& contents, std::string& hash, bool& missing,
                 std::string& error) const
    {
        missing = false;
        contents.clear();
        std::vector<std::string> hashes;
        for (const std::string& name : ContentFiles(r))
        {
            const auto* path = Find(name);
            if (!path)
            {
                missing = true;
                return true;
            }
            contents.emplace_back();
            if (!ReadWholeFile(*path, contents.back(), error))
                return false;
            hashes.push_back(Sha256Hex(contents.back()));
        }
        hash = r.form == Form::Kim ? KimContentHash(hashes[0], hashes[1]) : hashes[0];
        return true;
    }
};

} // namespace

bool GetStatus(const std::filesystem::path& clientData, unsigned jobs, Status& status, std::string& error)
{
    status = {};
    Workspace ws;
    if (!ws.Load(clientData, error))
        return false;

    const auto& records = ws.manifest.files;
    std::vector<char> state(records.size(), 0);  // 0 same, 1 changed, 2 missing
    if (!ParallelFor(records.size(), jobs, [&](std::size_t i, std::string& e) {
            std::vector<Bytes> contents;
            std::string hash;
            bool missing = false;
            if (!ws.Content(records[i], contents, hash, missing, e))
                return false;
            state[i] = missing ? 2 : records[i].encoding == Encoding::New ? 3 : hash == records[i].sha256 ? 0 : 1;
            return true;
        }, error))
        return false;

    for (std::size_t i = 0; i < records.size(); ++i)
        if (state[i] == 1)
            status.changed.push_back(records[i].name);
        else if (state[i] == 2)
            status.missing.push_back(records[i].name);
        else if (state[i] == 3)
            status.added.push_back(records[i].name);
    for (std::size_t i = 0; i < ws.files.size(); ++i)
        if (!ws.known[i])
            (ws.ignored[i] ? status.ignored : status.untracked).push_back(ws.files[i]);
    return true;
}

bool Pack(const PackOptions& options, PackStats& stats, std::string& error)
{
    stats = {};
    Log log(options.log);
    Workspace ws;
    if (!ws.Load(options.clientData, error))
        return false;

    std::error_code ec;
    if (!std::filesystem::create_directories(options.destination, ec) && ec)
    {
        error = "cannot create " + PathToUtf8(options.destination) + ": " + ec.message();
        return false;
    }
    if (SameFile(options.destination, options.clientData))
    {
        error = "pack into a folder other than client_data";
        return false;
    }

    // Records by archive, in index order.
    std::map<std::string, std::vector<const FileRecord*>> byArchive;
    for (const FileRecord& r : ws.manifest.files)
        byArchive[r.archive].push_back(&r);
    for (auto& [name, list] : byArchive)
        std::stable_sort(list.begin(), list.end(),
                         [](const FileRecord* a, const FileRecord* b) { return a->index < b->index; });

    // Each archive's fingerprint: its records, the hashes of their files now (cached by ClientData) and its header.
    // An archive whose fingerprint and destination file match the last pack's is left alone (onlyChanged).
    const auto& archives = ws.manifest.archives;
    std::vector<std::string> fingerprints(archives.size());
    {
        ClientData current;
        if (!current.Open(options.clientData, options.jobs, error))
            return false;
        for (std::size_t a = 0; a < archives.size(); ++a)
        {
            std::string text;
            if (const auto list = byArchive.find(archives[a].name); list != byArchive.end())
                for (const FileRecord* r : list->second)
                {
                    text += r->name + '\t' + std::to_string(r->index) + '\t' + std::to_string(r->algorithm) + '\t' +
                            std::string(ToString(r->encoding)) + '\t' + std::string(ToString(r->form)) + '\t' +
                            r->sha256;
                    for (const std::string& name : ContentFiles(*r))
                    {
                        const ClientDataFile* f = current.Find(name);
                        text += '\t' + (f && f->state != FileState::Deleted ? f->sha256 : std::string("-"));
                    }
                    text += '\n';
                }
            Bytes header;  // none for an archive created by a sync
            if (std::filesystem::exists(HeaderPath(options.clientData, archives[a].name)) &&
                !ReadWholeFile(HeaderPath(options.clientData, archives[a].name), header, error))
                return false;
            text += Sha256Hex(header);
            fingerprints[a] = Sha256Hex(ToBytes(text));
        }
    }
    struct Written
    {
        std::string fingerprint;
        std::uint64_t size = 0;
        std::int64_t time = 0;
    };
    std::map<std::string, Written> previous;
    {
        std::ifstream in(PackStatePath(options.destination), std::ios::binary);
        std::string line;
        if (std::getline(in, line) && line == "x2-assets-pack\t1")
            while (std::getline(in, line))
            {
                std::istringstream fields(line);
                std::string name;
                Written w;
                if (std::getline(fields, name, '\t') && std::getline(fields, w.fingerprint, '\t') && fields >> w.size >> w.time)
                    previous.emplace(name, w);
            }
    }
    const auto onDisk = [&](const std::string& archive) {
        Written w;
        std::error_code e2;
        const auto path = options.destination / PathFromUtf8(archive);
        w.size = std::filesystem::file_size(path, e2);
        if (e2)
            return std::optional<Written>();
        w.time = std::filesystem::last_write_time(path, e2).time_since_epoch().count();
        return std::optional<Written>(w);
    };
    std::vector<Written> written(archives.size());

    std::atomic<std::size_t> done{0}, identical{0}, files{0}, changed{0}, added{0}, missing{0}, notReproduced{0},
        luaSource{0}, kimCompiled{0}, skipped{0};
    const bool ok = ParallelFor(archives.size(), options.jobs, [&](std::size_t a, std::string& e) {
        const std::string& archiveName = archives[a].name;
        if (options.onlyChanged)
            if (const auto last = previous.find(archiveName); last != previous.end() &&
                                                              last->second.fingerprint == fingerprints[a])
                if (const auto now = onDisk(archiveName); now && now->size == last->second.size &&
                                                          now->time == last->second.time)
                {
                    written[a] = last->second;
                    ++skipped;
                    if (options.progress)
                        options.progress(++done, archives.size());
                    return true;
                }
        const auto list = byArchive.find(archiveName);
        kom::ArchiveBuilder builder;
        builder.SetLevel(kom::kKogLevel);
        Bytes data, blob;
        bool archiveChanged = false;
        if (list != byArchive.end())
            for (const FileRecord* r : list->second)
            {
                std::vector<Bytes> contents;
                std::string hash;
                bool isMissing = false;
                if (!ws.Content(*r, contents, hash, isMissing, e))
                    return false;
                if (isMissing)
                {
                    ++missing;
                    archiveChanged = true;
                    log(archiveName + ": " + r->name + " is missing from client_data: dropped");
                    continue;
                }
                const bool isNew = r->encoding == Encoding::New;  // no original: encoded from its content
                const bool same = !isNew && hash == r->sha256;
                const auto where = [&] { return archiveName + ": " + r->name + ": "; };
                if (same && r->form != Form::Raw)
                {
                    // the decompiled or converted file is unchanged: pack the original
                    if (!ReadWholeFile(OriginalPath(options.clientData, r->name), data, e))
                        return false;
                }
                else if (r->form == Form::Lua)
                {
                    // an edited script: Lua source with BOM, what the ported client loads (T21)
                    std::string source = ToText(contents[0]);
                    if (!lua::IsBytecode(contents[0]))
                    {
                        if (!source.starts_with(lua::kBom))
                            source.insert(0, lua::kBom);
                        std::string why;
                        if (!lua::CheckSyntax(source, r->name, why))
                        {
                            e = where() + "LuaJIT cannot load the edited script: " + why;
                            return false;
                        }
                    }
                    data = ToBytes(source);
                    ++luaSource;
                }
                else if (r->form == Form::Kim)
                {
                    std::vector<std::string> enumSources;
                    if (const auto* enumPath = ws.Find(kEnumScript))
                    {
                        Bytes enumBytes;
                        if (!ReadWholeFile(*enumPath, enumBytes, e))
                            return false;
                        if (!lua::IsBytecode(enumBytes))
                            enumSources.push_back(ToText(enumBytes));
                    }
                    std::vector<std::string> warnings;
                    std::string why;
                    if (!lua::CompileKim({ToText(contents[0]), ToText(contents[1])}, enumSources, data, warnings, why))
                    {
                        e = where() + "cannot compile the item scripts: " + why;
                        return false;
                    }
                    for (const std::string& w : warnings)
                        log(where() + w);
                    ++kimCompiled;
                }
                else
                {
                    data = std::move(contents[0]);
                }
                if (same && r->encoding == Encoding::Blob)
                {
                    if (!ReadWholeFile(BlobPath(options.clientData, r->name), blob, e))
                        return false;
                }
                else if (r->encoding == Encoding::Stored)
                {
                    if (!same)
                    {
                        e = where() + "changed, but algorithm " + std::to_string(r->algorithm) +
                            " cannot be encoded; restore the file";
                        return false;
                    }
                    blob = data;
                }
                else
                {
                    if (!kom::Encode(r->name, r->algorithm, data, blob, e, kom::kKogLevel))
                    {
                        e = where() + e;
                        return false;
                    }
                }
                if (same && (blob.size() != r->storedSize || kom::Adler32(blob) != r->checksum))
                {
                    if (r->encoding != Encoding::Zlib)
                    {
                        e = where() + "the kept original blob does not match the manifest";
                        return false;
                    }
                    ++notReproduced;
                    archiveChanged = true;
                    log(where() + "unchanged, but re-encoding gave different bytes (not zlib 1.3.1?)");
                }
                if (!same)
                {
                    ++(isNew ? added : changed);
                    archiveChanged = true;
                }
                if (data.size() > std::numeric_limits<std::uint32_t>::max())
                {
                    e = where() + "file too large";
                    return false;
                }
                // Stored: the file is the blob; its decoded size is only known from the manifest.
                const std::uint32_t size =
                    r->encoding == Encoding::Stored ? r->size : static_cast<std::uint32_t>(data.size());
                const std::optional<std::uint32_t> fileTime =
                    same ? std::optional<std::uint32_t>(r->fileTime) : std::nullopt;
                if (!builder.AddStored(r->name, r->algorithm, size, std::move(blob), e, fileTime))
                {
                    e = where() + e;
                    return false;
                }
                blob.clear();
                ++files;
            }

        Bytes header;  // an archive created by a sync has none: the builder generates it
        std::error_code exists;
        const bool hasHeader = std::filesystem::exists(HeaderPath(options.clientData, archiveName), exists);
        if (hasHeader && !ReadWholeFile(HeaderPath(options.clientData, archiveName), header, e))
            return false;
        const bool keepsHeader = hasHeader && builder.Describes(header);
        if (hasHeader)
            builder.SetHeader(std::move(header));
        if (keepsHeader)
            ++identical;
        if (!builder.Write(options.destination / PathFromUtf8(archiveName), e))
            return false;
        if (const auto now = onDisk(archiveName))
        {
            written[a] = *now;
            written[a].fingerprint = fingerprints[a];
        }
        if (options.progress)
            options.progress(done + 1, archives.size());
        log("[" + std::to_string(++done) + "/" + std::to_string(archives.size()) + "] " + archiveName +
            (archiveChanged ? " (changed)" : ""));
        return true;
    }, error);
    {
        // What was written, for the next onlyChanged pack (also after a failure: the archives that made it).
        std::string text = "x2-assets-pack\t1\n";
        for (std::size_t a = 0; a < archives.size(); ++a)
            if (!written[a].fingerprint.empty())
                text += archives[a].name + '\t' + written[a].fingerprint + '\t' + std::to_string(written[a].size) +
                        '\t' + std::to_string(written[a].time) + '\n';
        std::string ignored;
        if (!WriteWholeFile(PackStatePath(options.destination), ToBytes(text), ignored) && ok)
            log("cannot write " + PathToUtf8(PackStatePath(options.destination)) + ": the next pack writes everything");
    }
    if (!ok)
        return false;

    for (std::size_t i = 0; i < ws.files.size(); ++i)
        if (!ws.known[i] && ws.ignored[i])
            ++stats.ignored;
        else if (!ws.known[i])
        {
            ++stats.untracked;
            log("not packed (not in the manifest; x2_assets sync adds it): " + PathToUtf8(ws.files[i]));
        }
    stats.archives = archives.size();
    stats.identicalHeaders = identical;
    stats.files = files;
    stats.changed = changed;
    stats.added = added;
    stats.luaSource = luaSource;
    stats.kimCompiled = kimCompiled;
    stats.missing = missing;
    stats.notReproduced = notReproduced;
    stats.skipped = skipped;
    return true;
}

std::filesystem::path PackStatePath(const std::filesystem::path& destination)
{
    return destination / "x2_assets_pack.tsv";
}

} // namespace x2::assets
