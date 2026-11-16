// x2_assets: the client's assets as an organized folder, client_data (decisions T35 and T50,
// docs/tools/client-data.md). With no arguments, or a client_data folder, it opens the window; `--cli` (or a command
// name first) runs one command. `x2_assets --cli help` lists them, `x2_assets --cli help <command>` explains one.
#include "x2/assets/client_data.h"
#include "x2/assets/workspace.h"
#ifdef X2_ASSETS_WINDOW
#include "gui.h"
#endif
#include "x2/assets/texture_sizes.h"
#include "x2/kom/mount.h"
#include "x2/platform/console.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <map>
#include <set>
#include <span>
#include <string>
#include <vector>

using namespace x2::assets;

namespace {

struct Args
{
    std::vector<std::string> positional;
    std::map<std::string, std::string> options;  // --name=value, --flag ("")
    unsigned jobs = 0;
    bool ok = true;
    std::string bad;

    bool Has(const std::string& name) const { return options.contains(name); }
    std::string Get(const std::string& name) const
    {
        const auto it = options.find(name);
        return it == options.end() ? std::string() : it->second;
    }
};

struct Command
{
    const char* name;
    const char* synopsis;  // after "x2_assets --cli "
    const char* summary;   // one line, for the list
    const char* details;   // help <command>
    std::set<std::string> options;
    int (*run)(const Args&);
};

const std::vector<Command>& Commands();

const Command* FindCommand(const std::string& name)
{
    for (const Command& c : Commands())
        if (name == c.name)
            return &c;
    return nullptr;
}

int Fail(const std::string& error)
{
    std::fprintf(stderr, "x2_assets: %s\n", error.c_str());
    return 1;
}

int Usage(const Command& command)
{
    std::fprintf(stderr, "usage: x2_assets --cli %s\n(x2_assets --cli help %s explains it)\n", command.synopsis,
                 command.name);
    return 2;
}

void Print(const std::string& line)
{
    std::printf("%s\n", line.c_str());
    std::fflush(stdout);
}

double Seconds(std::chrono::steady_clock::time_point start)
{
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}

Args Parse(std::span<const std::string> argv)
{
    Args args;
    for (const std::string& a : argv)
    {
        try
        {
            if (a.starts_with("-j") && a.size() > 2)
                args.jobs = static_cast<unsigned>(std::stoul(a.substr(2)));
            else if (a.starts_with("--"))
            {
                const auto eq = a.find('=');
                args.options[a.substr(2, eq == std::string::npos ? std::string::npos : eq - 2)] =
                    eq == std::string::npos ? std::string() : a.substr(eq + 1);
            }
            else if (a.size() > 1 && a.front() == '-')
            {
                args.ok = false;
                args.bad = a;
            }
            else
                args.positional.push_back(a);
        }
        catch (const std::exception&)
        {
            args.ok = false;
            args.bad = a;
        }
    }
    return args;
}

// --- Commands on the .kom ---------------------------------------------------------------------------------------------

int RunImport(const Args& args)
{
    ImportOptions options;
    options.source = PathFromUtf8(args.positional[0]);
    options.destination = PathFromUtf8(args.positional[1]);
    options.jobs = args.jobs;
    options.readableLua = !args.Has("no-lua");
    options.log = Print;
    std::string error;
    if (args.Has("layout") && !options.layout.Load(PathFromUtf8(args.Get("layout")), error))
        return Fail(error);
    const auto start = std::chrono::steady_clock::now();
    ImportStats stats;
    if (!Import(options, stats, error))
        return Fail(error);
    std::printf("imported %zu archives, %zu files (%.1f MB), %zu original blobs kept, %zu stored as is; Lua: %zu "
                "decompiled, %zu kept as bytecode, %zu .kim converted; in %.0f s\n",
                stats.archives, stats.files, static_cast<double>(stats.bytes) / 1e6, stats.keptBlobs, stats.stored,
                stats.luaSource, stats.luaKept, stats.kim, Seconds(start));
    return 0;
}

int RunPack(const Args& args)
{
    PackOptions options;
    options.clientData = PathFromUtf8(args.positional[0]);
    options.destination = PathFromUtf8(args.positional[1]);
    options.jobs = args.jobs;
    options.onlyChanged = !args.Has("all");
    options.log = Print;
    const auto start = std::chrono::steady_clock::now();
    PackStats stats;
    std::string error;
    if (!Pack(options, stats, error))
        return Fail(error);
    std::printf("packed %zu archives (%zu left as they were, %zu with their original header), %zu files: %zu changed "
                "(%zu Lua as source, %zu .kim compiled), %zu new, %zu missing, %zu not in the manifest (sync adds "
                "them), %zu ignored, %zu not reproduced, in %.0f s\n",
                stats.archives - stats.skipped, stats.skipped, stats.identicalHeaders, stats.files, stats.changed,
                stats.luaSource, stats.kimCompiled, stats.added, stats.missing, stats.untracked, stats.ignored,
                stats.notReproduced, Seconds(start));
    return 0;
}

// Folder counts for a layout without extracting anything, and the most common name prefixes left unsorted.
int RunLayout(const Args& args)
{
    if (!args.Has("layout"))
        return Usage(*FindCommand("layout"));
    std::size_t shown = 20;
    if (args.Has("unsorted"))
        shown = std::stoul(args.Get("unsorted"));
    std::string error;
    Layout layout;
    if (!layout.Load(PathFromUtf8(args.Get("layout")), error))
        return Fail(error);
    std::vector<std::filesystem::path> paths;
    if (!x2::kom::ArchivesInLoadOrder(PathFromUtf8(args.positional[0]), paths, error))
        return Fail(error);
    std::map<std::string, std::size_t> folders, prefixes;
    std::size_t total = 0, unsorted = 0;
    for (const auto& path : paths)
    {
        auto archive = x2::kom::Archive::Open(path, error);
        if (!archive)
            return Fail(error);
        const std::string archiveName = PathToUtf8(path.filename());
        for (const auto& e : archive->Entries())
        {
            ++total;
            const std::string folder = layout.Classify(e.name, archiveName);
            if (folder.starts_with("_unsorted/"))
            {
                ++unsorted;
                ++folders["_unsorted/"];
                const std::string lower = x2::kom::AsciiLower(e.name);
                const auto dot = lower.rfind('.');
                const auto cut = lower.find('_');
                prefixes[archiveName + " " + lower.substr(0, std::min(cut, dot)) + " ." +
                         (dot == std::string::npos ? std::string() : lower.substr(dot + 1))]++;
            }
            else
                ++folders[folder];
        }
    }
    for (const auto& [folder, count] : folders)
        std::printf("%8zu  %s\n", count, folder.c_str());
    std::printf("%zu files, %zu unsorted (%.1f%%)\n", total, unsorted,
                total ? 100.0 * static_cast<double>(unsorted) / static_cast<double>(total) : 0.0);
    std::vector<std::pair<std::size_t, std::string>> top;
    for (const auto& [prefix, count] : prefixes)
        top.emplace_back(count, prefix);
    std::sort(top.rbegin(), top.rend());
    if (top.size() > shown)
        top.resize(shown);
    if (!top.empty())
        std::printf("most common unsorted (archive, name prefix, extension):\n");
    for (const auto& [count, prefix] : top)
        std::printf("%8zu  %s\n", count, prefix.c_str());
    return 0;
}

int RunTextureSizes(const Args& args)
{
    const auto start = std::chrono::steady_clock::now();
    TextureSizeStats stats;
    std::string error;
    if (!WriteTextureSizes(PathFromUtf8(args.positional[0]), PathFromUtf8(args.positional[1]), args.jobs, stats,
                           error))
        return Fail(error);
    std::printf("%zu image sizes written, %zu entries named as images skipped (no image), in %.0f s\n", stats.images,
                stats.skipped, Seconds(start));
    return 0;
}

// --- Commands on a client_data ----------------------------------------------------------------------------------------

bool OpenData(const Args& args, ClientData& data)
{
    std::string error;
    if (!data.Open(PathFromUtf8(args.positional[0]), args.jobs, error))
    {
        Fail(error);
        return false;
    }
    return true;
}

// Edits made by a command are recorded at once (the command line keeps nothing between runs): a sync.
int SyncAfter(ClientData& data, const std::string& done)
{
    SyncStats stats;
    std::string error;
    if (!data.Sync(stats, error))
        return Fail(error);
    data.SaveHashCache();
    std::printf("%s; manifest synced: %zu added, %zu dropped, %zu moved to another archive, %zu archives created\n",
                done.c_str(), stats.added, stats.removed, stats.moved, stats.archives);
    return 0;
}

std::string Shown(const ClientDataFile& f)
{
    return f.state == FileState::Deleted ? f.name : f.folder.empty() ? f.name : f.folder + "/" + f.name;
}

int RunStatus(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    std::set<FileState> shown = {FileState::Modified, FileState::Deleted, FileState::Untracked, FileState::Added};
    if (args.Has("all"))
        shown.insert({FileState::Unchanged, FileState::Ignored});
    if (args.Has("ignored"))
        shown.insert(FileState::Ignored);
    std::map<FileState, std::size_t> counts;
    for (const ClientDataFile& f : data.Files())
    {
        ++counts[f.state];
        if (!shown.contains(f.state) || args.Has("count"))
            continue;
        std::string archive;
        if (f.state == FileState::Untracked)
            archive = "  -> " + data.ArchiveOf(f);
        std::printf("%-9s %s%s\n", std::string(ToString(f.state)).c_str(), Shown(f).c_str(), archive.c_str());
    }
    data.SaveHashCache();
    std::printf("%zu modified, %zu deleted, %zu new (untracked), %zu added, %zu ignored, %zu unchanged\n",
                counts[FileState::Modified], counts[FileState::Deleted], counts[FileState::Untracked],
                counts[FileState::Added], counts[FileState::Ignored], counts[FileState::Unchanged]);
    return 0;
}

int RunInfo(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    int result = 0;
    for (std::size_t i = 1; i < args.positional.size(); ++i)
    {
        const ClientDataFile* f = data.Find(args.positional[i]);
        if (!f)
        {
            result = Fail(args.positional[i] + ": no file of that name in client_data or the manifest");
            continue;
        }
        std::printf("%s\n  state    %s\n", f->name.c_str(), std::string(ToString(f->state)).c_str());
        if (f->state != FileState::Deleted)
            std::printf("  path     %s\n  size     %llu\n  sha256   %s\n", Shown(*f).c_str(),
                        static_cast<unsigned long long>(f->size), f->sha256.c_str());
        if (f->record >= 0)
        {
            const FileRecord& r = data.GetManifest().files[static_cast<std::size_t>(f->record)];
            std::printf("  record   %s, entry %u of %s, algorithm %u, %s, %s\n", r.name.c_str(), r.index,
                        r.archive.c_str(), r.algorithm, std::string(ToString(r.encoding)).c_str(),
                        std::string(ToString(r.form)).c_str());
        }
        if (f->state != FileState::Deleted && f->state != FileState::Ignored)
            std::printf("  archive  %s%s\n", data.ArchiveOf(*f).c_str(),
                        f->record < 0 ? " (its folder's default, after a sync)" : "");
    }
    data.SaveHashCache();
    return result;
}

int RunWhere(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    const std::string& name = args.positional[1];
    if (const std::filesystem::path path = data.Locate(name); !path.empty())
    {
        std::printf("%s\n", PathToUtf8(path).c_str());
        return 0;
    }
    if (!args.Has("new"))
        return Fail(name + ": not in client_data (--new says where a new file of that name goes)");
    std::vector<std::string> siblings;
    for (std::size_t i = 2; i < args.positional.size(); ++i)
        siblings.push_back(args.positional[i]);
    const std::string folder = data.FolderForNew(name, siblings);
    std::printf("%s\n", PathToUtf8(data.Root() / PathFromUtf8(folder) / PathFromUtf8(name)).c_str());
    return 0;
}

int RunArchives(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    std::map<std::string, std::size_t> files;
    for (const ClientDataFile& f : data.Files())
        if (f.state != FileState::Deleted && f.state != FileState::Ignored)
            ++files[data.ArchiveOf(f)];
    for (const std::string& archive : data.Archives())
        std::printf("%8zu  %s\n", files[archive], archive.c_str());
    return 0;
}

int RunAdd(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    std::size_t added = 0;
    std::string error;
    for (std::size_t i = 1; i < args.positional.size(); ++i)
    {
        const std::filesystem::path source = PathFromUtf8(args.positional[i]);
        const std::string folder = args.Has("to") ? args.Get("to") : data.FolderForNew(PathToUtf8(source.filename()));
        if (!data.AddFile(source, folder, error))
            return Fail(error);
        std::printf("added %s\n", PathToUtf8(PathFromUtf8(folder) / source.filename()).c_str());
        ++added;
    }
    if (args.Has("no-sync"))
    {
        data.SaveHashCache();
        return 0;
    }
    return SyncAfter(data, std::to_string(added) + " files added");
}

int RunRemove(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    std::string error;
    for (std::size_t i = 1; i < args.positional.size(); ++i)
        if (!data.RemoveFile(args.positional[i], error))
            return Fail(error);
    if (args.Has("no-sync"))
    {
        data.SaveHashCache();
        return 0;
    }
    return SyncAfter(data, std::to_string(args.positional.size() - 1) + " files removed");
}

// A file by name, else a folder by its path.
bool MoveOne(ClientData& data, const std::string& what, const std::string& folder, const std::string& newName,
             std::string& error)
{
    if (const ClientDataFile* f = data.Find(what); f && f->state != FileState::Deleted)
        return data.MoveFile(f->name, folder, error, newName);
    if (data.Folders().contains(what))
        return data.MoveFolder(what, folder, error, newName);
    error = what + ": neither a file name nor a folder of client_data";
    return false;
}

int RunMove(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    const std::string& to = args.positional.back();
    std::string error;
    for (std::size_t i = 1; i + 1 < args.positional.size(); ++i)
    {
        if (!MoveOne(data, args.positional[i], to == "." ? "" : to, {}, error))
            return Fail(error);
        std::printf("moved %s to %s\n", args.positional[i].c_str(), to.c_str());
    }
    data.SaveHashCache();
    return 0;
}

int RunRename(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    const std::string& what = args.positional[1];
    std::string folder;
    if (const ClientDataFile* f = data.Find(what); f && f->state != FileState::Deleted)
        folder = f->folder;
    else
    {
        const auto slash = what.rfind('/');
        folder = slash == std::string::npos ? std::string() : what.substr(0, slash);
    }
    std::string error;
    if (!MoveOne(data, what, folder, args.positional[2], error))
        return Fail(error);
    data.SaveHashCache();
    std::printf("renamed %s to %s\n", what.c_str(), args.positional[2].c_str());
    return 0;
}

int RunMkdir(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    std::string error;
    for (std::size_t i = 1; i < args.positional.size(); ++i)
        if (!data.CreateFolder(args.positional[i], error))
            return Fail(error);
    return 0;
}

int RunSetArchive(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    std::string archive = args.positional.back();
    std::string error;
    if (args.Has("new"))
    {
        if (!x2::kom::AsciiLower(archive).ends_with(".kom"))
            archive += ".kom";
        if (!data.CreateArchive(archive, error))
            return Fail(error);
    }
    std::size_t count = 0;
    for (std::size_t i = 1; i + 1 < args.positional.size(); ++i)
    {
        const std::string& what = args.positional[i];
        if (const ClientDataFile* f = data.Find(what); f && f->state != FileState::Deleted)
        {
            if (!data.SetArchive(f->name, archive, error))
                return Fail(error);
            ++count;
        }
        else if (data.Folders().contains(what) || what == ".")
        {
            std::size_t n = 0;
            if (!data.SetFolderArchive(what == "." ? "" : what, archive, n, error))
                return Fail(error);
            count += n;
        }
        else
            return Fail(what + ": neither a file name nor a folder of client_data");
    }
    return SyncAfter(data, std::to_string(count) + " files set to " + archive);
}

int RunIgnore(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    std::string error;
    if (args.positional.size() == 1)
    {
        for (const std::string& pattern : data.Ignores().patterns)
            std::printf("%s\n", pattern.c_str());
        return 0;
    }
    for (std::size_t i = 1; i < args.positional.size(); ++i)
    {
        const std::string& what = args.positional[i];
        const bool ok = data.Folders().contains(what) ? data.IgnoreFolder(what, error) : data.Ignore(what, error);
        if (!ok)
            return Fail(error);
    }
    data.SaveHashCache();
    std::printf("%zu patterns in .kom/ignore.txt\n", data.Ignores().patterns.size());
    return 0;
}

int RunUnignore(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    std::string error;
    for (std::size_t i = 1; i < args.positional.size(); ++i)
        if (!data.Unignore(args.positional[i], error))
            return Fail(error);
    data.SaveHashCache();
    std::printf("%zu patterns in .kom/ignore.txt\n", data.Ignores().patterns.size());
    return 0;
}

int RunSync(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    return SyncAfter(data, "structure");
}

int RunCommit(const Args& args)
{
    ClientData data;
    if (!OpenData(args, data))
        return 1;
    const auto start = std::chrono::steady_clock::now();
    ClientData::CommitStats stats;
    std::string error;
    if (!data.Commit(args.jobs, stats, error))
        return Fail(error);
    data.SaveHashCache();
    std::printf("committed: %zu files took their content as the original; %zu added, %zu dropped, %zu moved to "
                "another archive, %zu archives created; in %.0f s\n",
                stats.committed, stats.sync.added, stats.sync.removed, stats.sync.moved, stats.sync.archives,
                Seconds(start));
    return 0;
}

int RunHelp(const Args& args);

const std::vector<Command>& Commands()
{
    static const std::vector<Command> commands = {
        {"help", "help [<command>]", "lists the commands, or explains one",
         "Without a command, lists every command; with one, explains it with examples.\n\n"
         "  x2_assets --cli help\n  x2_assets --cli help pack\n",
         {}, RunHelp},
        {"import", "import <client data folder> <client_data> [--layout=<rules>] [--no-lua] [-j<N>]",
         "extracts a client's .kom into a new client_data",
         "Extracts every .kom of a client's data folder into client_data, a folder that must not exist or be empty,\n"
         "organized by the layout rules (tools/assets/layout.txt; without them everything goes to _unsorted/), and\n"
         "writes its manifest (.kom/manifest.tsv). Lua bytecode is decompiled and Item.kim becomes Item.lua +\n"
         "SetItem.lua, unless --no-lua. -j<N>: threads (default: one per hardware thread).\n\n"
         "  x2_assets --cli import /games/elsword/data client_data --layout=tools/assets/layout.txt\n",
         {"layout", "no-lua"}, RunImport},
        {"status", "status <client_data> [--all] [--ignored] [--count] [-j<N>]",
         "lists what changed in client_data against the manifest",
         "Lists the files that differ from the manifest, one per line with its state:\n"
         "  modified   its content differs from the original (commit makes it the original)\n"
         "  deleted    in the manifest, not in client_data (sync or commit drops it)\n"
         "  untracked  in client_data, not in the manifest (sync adds it; shown with the archive it goes to)\n"
         "  added      in the manifest as a new file, in no original archive\n"
         "--ignored also lists the ignored files, --all every file, --count only the totals. Hashes are cached in\n"
         ".kom/hashes.tsv, so only files whose size or time changed are read again.\n\n"
         "  x2_assets --cli status client_data\n",
         {"all", "ignored", "count"}, RunStatus},
        {"info", "info <client_data> <name>...", "shows a file's state, place, record and archive",
         "Shows what client_data knows of files, found by name (any folder, ASCII case-insensitive): state, path,\n"
         "size, hash, manifest record and the archive it goes to.\n\n"
         "  x2_assets --cli info client_data DLG_Login3.lua ItemTemplet.lua\n",
         {}, RunInfo},
        {"where", "where <client_data> <name> [--new [<sibling>...]]",
         "prints a file's path, found by name (for tools and scripts)",
         "Prints the path of the file of that name, wherever client_data has it: tools find files this way, never\n"
         "by a fixed path, so client_data can be reorganized at any time. With --new, a file that does not exist\n"
         "yet gets the path where it should be created: the folder of the first sibling found (a file it belongs\n"
         "with), else the root. Fails (exit 1) if the file is not there and --new is not given.\n\n"
         "  x2_assets --cli where client_data ItemTemplet.lua\n"
         "  x2_assets --cli where client_data UIAnchors.lua --new DLG_Login3.lua\n",
         {"new"}, RunWhere},
        {"archives", "archives <client_data>", "lists the archives in load order with their file counts",
         "Lists the archives of the manifest in the client's load order, each with the files it will hold after a\n"
         "sync. The client loads only its own list (data001-150, datai001-023); other archives are kept by the tool\n"
         "but not read by the game.\n\n  x2_assets --cli archives client_data\n",
         {}, RunArchives},
        {"add", "add <client_data> <file>... [--to=<folder>] [--no-sync]", "copies files into client_data",
         "Copies files into client_data, into --to=<folder> (relative to client_data) or, by default, where a file\n"
         "of that name already is, else the root. The manifest records them at once (a sync) unless --no-sync; their\n"
         "archive is the folder's default (its most common archive).\n\n"
         "  x2_assets --cli add client_data ~/new/DLG_Shop.lua --to=ui/scripts\n",
         {"to", "no-sync"}, RunAdd},
        {"remove", "remove <client_data> <name>... [--no-sync]", "deletes files from client_data, by name",
         "Deletes files (found by name) from client_data for good. The manifest drops them at once (a sync) unless\n"
         "--no-sync; until then they show as deleted.\n\n  x2_assets --cli remove client_data Old_Effect.dds\n",
         {"no-sync"}, RunRemove},
        {"move", "move <client_data> <name or folder>... <to folder>", "moves files or folders inside client_data",
         "Moves files (by name) or folders (by path) into another folder of client_data (\".\" for the root);\n"
         "folders are created as needed. The manifest does not change: it holds no paths.\n\n"
         "  x2_assets --cli move client_data DLG_Shop.lua DLG_Bank.lua ui/scripts/town\n"
         "  x2_assets --cli move client_data effects/old _attic\n",
         {}, RunMove},
        {"rename", "rename <client_data> <name or folder> <new name>", "renames a file or a folder",
         "Renames a file (by name) or a folder (by path) where it is. The game finds files by name: renaming a file\n"
         "breaks whatever loads the old name, and the manifest sees it as deleted plus a new file.\n\n"
         "  x2_assets --cli rename client_data ui/scripts/town ui_town\n",
         {}, RunRename},
        {"mkdir", "mkdir <client_data> <folder>...", "creates folders in client_data",
         "Creates folders (relative to client_data, with their parents).\n\n"
         "  x2_assets --cli mkdir client_data ui/scripts/new\n",
         {}, RunMkdir},
        {"set-archive", "set-archive <client_data> <name or folder>... <archive> [--new]",
         "chooses the .kom files go to",
         "Sets the archive that files (by name) or every file under folders (by path; \".\" for all) go to, and\n"
         "records it in the manifest at once (a sync, which also adds new files and drops deleted ones). --new\n"
         "creates the archive first (\".kom\" is added if missing); its place in the load order is its name's.\n\n"
         "  x2_assets --cli set-archive client_data ui/scripts data012.kom\n"
         "  x2_assets --cli set-archive client_data DLG_Shop.lua data151.kom --new\n",
         {"new"}, RunSetArchive},
        {"ignore", "ignore <client_data> [<name or folder>...]", "keeps new files out of the manifest and the .kom",
         "Adds files (by name) or folders (by path) to .kom/ignore.txt: new files they match are never synced,\n"
         "committed or packed (notes, layouts, tools kept in client_data for convenience). Without names, lists the\n"
         "patterns. Patterns ending in / are folders, with / globs on the path, otherwise globs on the file name.\n\n"
         "  x2_assets --cli ignore client_data layout.txt x2_assets _notes\n",
         {}, RunIgnore},
        {"unignore", "unignore <client_data> <name>...", "stops ignoring files",
         "Removes the patterns of .kom/ignore.txt that match each file; fails if one is a pattern of more files\n"
         "(edit .kom/ignore.txt then).\n\n  x2_assets --cli unignore client_data layout.txt\n",
         {}, RunUnignore},
        {"sync", "sync <client_data> [-j<N>]", "writes the manifest's structure (new and deleted files)",
         "\"Sync data structure\": new files get a record (in their folder's default archive), deleted files' records\n"
         "are dropped. Modified files stay modified: the manifest keeps their original (commit changes that). The\n"
         "previous manifest is kept as .kom/manifest.tsv.bak.\n\n  x2_assets --cli sync client_data\n",
         {}, RunSync},
        {"commit", "commit <client_data> [-j<N>]", "makes client_data's content the original",
         "The .kom are the source of truth and client_data the workspace: commit makes the workspace the truth. A\n"
         "sync, then every modified and new file becomes the original of its record (encoded as pack would);\n"
         "afterwards status shows nothing. Nothing is packed: pack writes the .kom. The previous manifest is kept\n"
         "as .kom/manifest.tsv.bak.\n\n  x2_assets --cli commit client_data\n",
         {}, RunCommit},
        {"pack", "pack <client_data> <destination> [--all] [-j<N>]", "writes the .kom of the manifest",
         "Writes the archives of the manifest into the destination folder (created if needed), from client_data's\n"
         "files: unchanged content byte for byte as imported, edited files re-encoded, Lua checked by LuaJIT. Only\n"
         "the archives whose content changed since the last pack into that destination are written\n"
         "(x2_assets_pack.tsv there), unless --all. Pack does not change the manifest; files it does not know\n"
         "(untracked) are left out: sync or commit first.\n\n"
         "  x2_assets --cli pack client_data /games/elsword/data\n",
         {"all"}, RunPack},
        {"layout", "layout <client data folder> --layout=<rules> [--unsorted=<count>]",
         "shows where import would put a client's files",
         "Counts the files per folder that import would make with these layout rules, without extracting, and lists\n"
         "the most common name prefixes left in _unsorted/ (--unsorted=<count>, default 20).\n\n"
         "  x2_assets --cli layout /games/elsword/data --layout=tools/assets/layout.txt\n",
         {"layout", "unsorted"}, RunLayout},
        {"texture-sizes", "texture-sizes <client data folder> <output file> [-j<N>]",
         "lists the original size of every image",
         "Writes the original size of every image of a client's .kom (data/OriginalTextureSizes.txt), so the client\n"
         "can lay out replaced HD textures at their original size.\n\n"
         "  x2_assets --cli texture-sizes /games/elsword/data OriginalTextureSizes.txt\n",
         {}, RunTextureSizes},
    };
    return commands;
}

// Positional arguments each command takes: at least, at most (0: any).
std::pair<std::size_t, std::size_t> Arity(const std::string& name)
{
    static const std::map<std::string, std::pair<std::size_t, std::size_t>> arity = {
        {"help", {0, 1}},     {"import", {2, 2}},      {"status", {1, 1}},  {"info", {2, 0}},
        {"where", {2, 0}},    {"archives", {1, 1}},    {"add", {2, 0}},     {"remove", {2, 0}},
        {"move", {3, 0}},     {"rename", {3, 3}},      {"mkdir", {2, 0}},   {"set-archive", {3, 0}},
        {"ignore", {1, 0}},   {"unignore", {2, 0}},    {"sync", {1, 1}},    {"commit", {1, 1}},
        {"pack", {2, 2}},     {"layout", {1, 1}},      {"texture-sizes", {2, 2}},
    };
    return arity.at(name);
}

int List()
{
    std::printf("x2_assets: the client's assets as a folder, client_data (docs/tools/client-data.md)\n\n"
                "  x2_assets [<client_data>]            opens the window\n"
                "  x2_assets --cli <command> <args>     runs a command\n\n"
                "Commands:\n");
    for (const Command& c : Commands())
        std::printf("  %-14s %s\n", c.name, c.summary);
    std::printf("\nThe usual cycle: import once; edit files in client_data; status to see the changes; sync (new and\n"
                "deleted files) or commit (everything becomes the original); pack into the client's data folder.\n"
                "Files are found by name, never by a fixed path (where). Run x2_assets --cli help <command> for one.\n");
    return 0;
}

int RunHelp(const Args& args)
{
    if (args.positional.empty())
        return List();
    const Command* c = FindCommand(args.positional[0]);
    if (!c)
        return Fail("no command " + args.positional[0] + " (x2_assets --cli help lists them)");
    std::printf("x2_assets --cli %s\n\n%s", c->synopsis, c->details);
    return 0;
}

int RunCommand(const std::vector<std::string>& argv)
{
    if (argv.empty())
        return List();
    const Command* command = FindCommand(argv[0]);
    if (!command)
    {
        std::fprintf(stderr, "x2_assets: no command %s\n\n", argv[0].c_str());
        List();
        return 2;
    }
    const Args args = Parse(std::span(argv).subspan(1));
    if (!args.ok)
    {
        std::fprintf(stderr, "x2_assets: %s: bad option %s\n", command->name, args.bad.c_str());
        return Usage(*command);
    }
    for (const auto& [option, value] : args.options)
    {
        if (option == "help")
            return RunHelp(Args{{command->name}, {}, 0, true, {}});
        if (!command->options.contains(option))
        {
            std::fprintf(stderr, "x2_assets: %s: no option --%s\n", command->name, option.c_str());
            return Usage(*command);
        }
    }
    const auto [least, most] = Arity(command->name);
    if (args.positional.size() < least || (most && args.positional.size() > most))
        return Usage(*command);
    try
    {
        return command->run(args);
    }
    catch (const std::exception& e)
    {
        return Fail(std::string(command->name) + ": " + e.what());
    }
}

} // namespace

int main(int argc, char* argv[])
{
    x2::platform::InitConsole();
    std::vector<std::string> argv8 = x2::platform::Utf8Arguments(argc, argv);
    argv8.erase(argv8.begin());
    const bool cli = !argv8.empty() && (argv8[0] == "--cli" || FindCommand(argv8[0]) || argv8[0] == "--help" ||
                                        argv8[0] == "-h");
    if (!argv8.empty() && argv8[0] == "--cli")
        argv8.erase(argv8.begin());
    if (!argv8.empty() && (argv8[0] == "--help" || argv8[0] == "-h"))
        argv8[0] = "help";
#ifdef X2_ASSETS_WINDOW
    if (!cli && argv8.size() <= 1)
        return RunWindow(argv8.empty() ? std::filesystem::path() : PathFromUtf8(argv8[0]));
#endif
    return RunCommand(argv8);
}
