#include "x2/assets/client_data.h"

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
#include <span>
#include <thread>
#include <unordered_map>

namespace x2::assets {

namespace {

using Bytes = std::vector<std::uint8_t>;

unsigned Jobs(unsigned requested, std::size_t work)
{
    unsigned jobs = requested ? requested : std::max(1u, std::thread::hardware_concurrency());
    return static_cast<unsigned>(std::min<std::size_t>(jobs, std::max<std::size_t>(work, 1)));
}

// Runs fn(i) for i in [0, count) on `jobs` threads; stops handing out work at the first failure and returns its
// error.
bool ParallelFor(std::size_t count, unsigned jobs, const std::function<bool(std::size_t, std::string&)>& fn,
                 std::string& error)
{
    std::atomic<std::size_t> next{0};
    std::atomic<bool> failed{false};
    std::mutex mutex;
    const auto worker = [&] {
        std::string e;
        while (!failed)
        {
            const std::size_t i = next++;
            if (i >= count)
                return;
            if (!fn(i, e))
            {
                const std::lock_guard lock(mutex);
                if (!failed.exchange(true))
                    error = e;
                return;
            }
        }
    };
    jobs = Jobs(jobs, count);
    std::vector<std::thread> threads;
    for (unsigned k = 1; k < jobs; ++k)
        threads.emplace_back(worker);
    worker();
    for (std::thread& t : threads)
        t.join();
    return !failed;
}

// Serializes a LogSink called from several threads.
class Log
{
public:
    explicit Log(const LogSink& sink) : sink_(sink) {}
    void operator()(const std::string& line)
    {
        if (!sink_)
            return;
        const std::lock_guard lock(mutex_);
        sink_(line);
    }

private:
    const LogSink& sink_;
    std::mutex mutex_;
};

bool ReadWholeFile(const std::filesystem::path& path, Bytes& out, std::string& error)
{
    std::ifstream in(path, std::ios::binary);
    if (!in)
    {
        error = "cannot open " + PathToUtf8(path);
        return false;
    }
    in.seekg(0, std::ios::end);
    const auto size = static_cast<std::size_t>(in.tellg());
    in.seekg(0);
    out.resize(size);
    if (size && !in.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(size)))
    {
        error = "cannot read " + PathToUtf8(path);
        return false;
    }
    return true;
}

bool WriteWholeFile(const std::filesystem::path& path, std::span<const std::uint8_t> data, std::string& error)
{
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (out)
        out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    out.flush();
    if (!out)
    {
        error = "cannot write " + PathToUtf8(path);
        return false;
    }
    return true;
}

std::string Sha256Hex(std::span<const std::uint8_t> data)
{
    static constexpr char kDigits[] = "0123456789abcdef";
    const auto digest = crypto::Sha256::Hash(data.data(), data.size());
    std::string out;
    out.reserve(digest.size() * 2);
    for (const auto b : digest)
    {
        out += kDigits[b >> 4];
        out += kDigits[b & 0xF];
    }
    return out;
}

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

// The content hash of a kim record: of its two scripts' hashes.
std::string KimContentHash(const std::string& itemsSha, const std::string& setsSha)
{
    const std::string both = itemsSha + setsSha;
    return Sha256Hex(std::span(reinterpret_cast<const std::uint8_t*>(both.data()), both.size()));
}

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

    bool Load(const std::filesystem::path& clientData, std::string& error)
    {
        if (!manifest.Read(ManifestPath(clientData), error) || !IndexFiles(clientData, files, error))
            return false;
        for (std::size_t i = 0; i < files.size(); ++i)
            byName.emplace(kom::AsciiLower(PathToUtf8(files[i].filename())), i);
        known.assign(files.size(), false);
        for (const FileRecord& r : manifest.files)
            for (const std::string& name : ContentFiles(r))
                if (const auto it = byName.find(kom::AsciiLower(name)); it != byName.end())
                    known[it->second] = true;
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
            state[i] = missing ? 2 : hash == records[i].sha256 ? 0 : 1;
            return true;
        }, error))
        return false;

    for (std::size_t i = 0; i < records.size(); ++i)
        if (state[i] == 1)
            status.changed.push_back(records[i].name);
        else if (state[i] == 2)
            status.missing.push_back(records[i].name);
    for (std::size_t i = 0; i < ws.files.size(); ++i)
        if (!ws.known[i])
            status.added.push_back(ws.files[i]);
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

    std::atomic<std::size_t> done{0}, identical{0}, files{0}, changed{0}, missing{0}, notReproduced{0}, luaSource{0},
        kimCompiled{0};
    const auto& archives = ws.manifest.archives;
    const bool ok = ParallelFor(archives.size(), options.jobs, [&](std::size_t a, std::string& e) {
        const std::string& archiveName = archives[a].name;
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
                const bool same = hash == r->sha256;
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
                    ++changed;
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

        Bytes header;
        if (!ReadWholeFile(HeaderPath(options.clientData, archiveName), header, e))
            return false;
        const bool keepsHeader = builder.Describes(header);
        builder.SetHeader(std::move(header));
        if (keepsHeader)
            ++identical;
        if (!builder.Write(options.destination / PathFromUtf8(archiveName), e))
            return false;
        log("[" + std::to_string(++done) + "/" + std::to_string(archives.size()) + "] " + archiveName +
            (archiveChanged ? " (changed)" : ""));
        return true;
    }, error);
    if (!ok)
        return false;

    for (std::size_t i = 0; i < ws.files.size(); ++i)
        if (!ws.known[i])
        {
            ++stats.added;
            log("not packed (new files are not assigned to an archive yet): " + PathToUtf8(ws.files[i]));
        }
    stats.archives = archives.size();
    stats.identicalHeaders = identical;
    stats.files = files;
    stats.changed = changed;
    stats.luaSource = luaSource;
    stats.kimCompiled = kimCompiled;
    stats.missing = missing;
    stats.notReproduced = notReproduced;
    return true;
}

} // namespace x2::assets
