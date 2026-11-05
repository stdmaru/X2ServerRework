// x2_assets: the client's assets as an organized folder (decision T35, docs/tools/client-data.md).
//   x2_assets import <client data folder> <client_data> [--layout=<rules>] [-j<N>]
//   x2_assets pack <client_data> <output folder> [-j<N>]
//   x2_assets status <client_data> [-j<N>]
//   x2_assets layout <client data folder> --layout=<rules> [--unsorted=<count>]
#include "x2/assets/client_data.h"
#include "x2/kom/mount.h"
#include "x2/platform/console.h"

#include <chrono>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

using namespace x2::assets;

namespace {

int Usage()
{
    std::fputs("usage: x2_assets import <client data folder> <client_data> [--layout=<rules>] [-j<N>]\n"
               "       x2_assets pack <client_data> <output folder> [-j<N>]\n"
               "       x2_assets status <client_data> [-j<N>]\n"
               "       x2_assets layout <client data folder> --layout=<rules> [--unsorted=<count>]\n"
               "import extracts the .kom of a client's data folder into client_data, organized by the layout rules\n"
               "(tools/assets/layout.txt), with a manifest; pack writes them back, byte for byte when nothing\n"
               "changed; status lists what changed; layout shows where import would put the files.\n",
               stderr);
    return 2;
}

int Fail(const std::string& error)
{
    std::fprintf(stderr, "x2_assets: %s\n", error.c_str());
    return 1;
}

void Print(const std::string& line)
{
    std::printf("%s\n", line.c_str());
    std::fflush(stdout);
}

struct Args
{
    std::vector<std::string> positional;
    std::string layout;
    unsigned jobs = 0;
    std::size_t unsorted = 20;
    bool ok = true;
};

Args Parse(const std::vector<std::string>& argv)
{
    Args args;
    for (std::size_t i = 2; i < argv.size(); ++i)
    {
        const std::string& a = argv[i];
        try
        {
            if (a.rfind("--layout=", 0) == 0)
                args.layout = a.substr(9);
            else if (a.rfind("--unsorted=", 0) == 0)
                args.unsorted = std::stoul(a.substr(11));
            else if (a.rfind("-j", 0) == 0 && a.size() > 2)
                args.jobs = static_cast<unsigned>(std::stoul(a.substr(2)));
            else if (!a.empty() && a.front() == '-')
                args.ok = false;
            else
                args.positional.push_back(a);
        }
        catch (const std::exception&)
        {
            args.ok = false;
        }
    }
    return args;
}

double Seconds(std::chrono::steady_clock::time_point start)
{
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}

int RunImport(const Args& args)
{
    if (args.positional.size() != 2)
        return Usage();
    ImportOptions options;
    options.source = PathFromUtf8(args.positional[0]);
    options.destination = PathFromUtf8(args.positional[1]);
    options.jobs = args.jobs;
    options.log = Print;
    std::string error;
    if (!args.layout.empty() && !options.layout.Load(PathFromUtf8(args.layout), error))
        return Fail(error);
    const auto start = std::chrono::steady_clock::now();
    ImportStats stats;
    if (!Import(options, stats, error))
        return Fail(error);
    std::printf("imported %zu archives, %zu files (%.1f MB), %zu original blobs kept, %zu stored as is, in %.0f s\n",
                stats.archives, stats.files, static_cast<double>(stats.bytes) / 1e6, stats.keptBlobs, stats.stored,
                Seconds(start));
    return 0;
}

int RunPack(const Args& args)
{
    if (args.positional.size() != 2)
        return Usage();
    PackOptions options;
    options.clientData = PathFromUtf8(args.positional[0]);
    options.destination = PathFromUtf8(args.positional[1]);
    options.jobs = args.jobs;
    options.log = Print;
    const auto start = std::chrono::steady_clock::now();
    PackStats stats;
    std::string error;
    if (!Pack(options, stats, error))
        return Fail(error);
    std::printf("packed %zu archives (%zu with their original header), %zu files: %zu changed, %zu missing, "
                "%zu new not packed, %zu not reproduced, in %.0f s\n",
                stats.archives, stats.identicalHeaders, stats.files, stats.changed, stats.missing, stats.added,
                stats.notReproduced, Seconds(start));
    return 0;
}

int RunStatus(const Args& args)
{
    if (args.positional.size() != 1)
        return Usage();
    Status status;
    std::string error;
    if (!GetStatus(PathFromUtf8(args.positional[0]), args.jobs, status, error))
        return Fail(error);
    for (const auto& name : status.changed)
        std::printf("changed  %s\n", name.c_str());
    for (const auto& name : status.missing)
        std::printf("missing  %s\n", name.c_str());
    for (const auto& path : status.added)
        std::printf("new      %s\n", PathToUtf8(path).c_str());
    std::printf("%zu changed, %zu missing, %zu new\n", status.changed.size(), status.missing.size(),
                status.added.size());
    return 0;
}

// Folder counts for a layout without extracting anything, and the most common name prefixes left unsorted.
int RunLayout(const Args& args)
{
    if (args.positional.size() != 1 || args.layout.empty())
        return Usage();
    std::string error;
    Layout layout;
    if (!layout.Load(PathFromUtf8(args.layout), error))
        return Fail(error);
    std::vector<std::filesystem::path> paths;
    if (!x2::kom::ArchivesInLoadOrder(PathFromUtf8(args.positional[0]), paths, error))
        return Fail(error);
    std::map<std::string, std::size_t> folders, prefixes;
    std::vector<std::size_t> hits(layout.Rules().size(), 0);
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
            if (folder.rfind("_unsorted/", 0) == 0)
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
            {
                ++folders[folder];
            }
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
    if (top.size() > args.unsorted)
        top.resize(args.unsorted);
    if (!top.empty())
        std::printf("most common unsorted (archive, name prefix, extension):\n");
    for (const auto& [count, prefix] : top)
        std::printf("%8zu  %s\n", count, prefix.c_str());
    return 0;
}

} // namespace

int main(int argc, char* argv[])
{
    x2::platform::InitConsole();
    const std::vector<std::string> argv8 = x2::platform::Utf8Arguments(argc, argv);
    if (argv8.size() < 2)
        return Usage();
    const Args args = Parse(argv8);
    if (!args.ok)
        return Usage();
    const std::string& command = argv8[1];
    if (command == "import")
        return RunImport(args);
    if (command == "pack")
        return RunPack(args);
    if (command == "status")
        return RunStatus(args);
    if (command == "layout")
        return RunLayout(args);
    return Usage();
}
