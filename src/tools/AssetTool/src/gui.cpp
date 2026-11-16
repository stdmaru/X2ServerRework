#include "gui.h"

#include "x2/app/imgui_layer.h"
#include "x2/assets/client_data.h"
#include "x2/assets/workspace.h"
#include "x2/kom/codec.h"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <thread>
#include <unordered_map>

namespace x2::assets {

namespace {

constexpr std::size_t kStates = 6;

std::size_t Index(FileState s) { return static_cast<std::size_t>(s); }

ImVec4 StateColor(FileState state)
{
    switch (state)
    {
    case FileState::Modified: return {0.95f, 0.80f, 0.30f, 1.0f};
    case FileState::Deleted: return {0.95f, 0.40f, 0.40f, 1.0f};
    case FileState::Untracked: return {0.45f, 0.85f, 0.45f, 1.0f};
    case FileState::Added: return {0.50f, 0.75f, 1.00f, 1.0f};
    case FileState::Ignored: return ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
    case FileState::Unchanged: break;
    }
    return ImGui::GetStyleColorVec4(ImGuiCol_Text);
}

const char* StateLabel(FileState state)
{
    switch (state)
    {
    case FileState::Unchanged: return "unchanged";
    case FileState::Modified: return "modified (differs from the original)";
    case FileState::Deleted: return "deleted (a sync drops it from the manifest)";
    case FileState::Untracked: return "new, not in the manifest (a sync adds it)";
    case FileState::Added: return "new, in the manifest";
    case FileState::Ignored: return "ignored (.kom/ignore.txt): never synced, committed or packed";
    }
    return "?";
}

std::string Lower(std::string_view s) { return kom::AsciiLower(s); }

std::string FormatSize(std::uint64_t bytes)
{
    char text[64];
    if (bytes >= 1024ull * 1024)
        std::snprintf(text, sizeof text, "%.1f MB", static_cast<double>(bytes) / (1024.0 * 1024.0));
    else if (bytes >= 1024)
        std::snprintf(text, sizeof text, "%.1f KB", static_cast<double>(bytes) / 1024.0);
    else
        std::snprintf(text, sizeof text, "%llu bytes", static_cast<unsigned long long>(bytes));
    return text;
}

std::string Join(const std::string& folder, std::string_view name)
{
    return folder.empty() ? std::string(name) : folder + "/" + std::string(name);
}

std::string Parent(const std::string& path)
{
    const auto slash = path.rfind('/');
    return slash == std::string::npos ? std::string() : path.substr(0, slash);
}

std::string LastPart(const std::string& path)
{
    const auto slash = path.rfind('/');
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

// What the window remembers between runs (the last client_data, the pack destination), one file each.
std::filesystem::path PrefFile(const char* name)
{
    char* pref = SDL_GetPrefPath("x2", "x2_assets");
    if (!pref)
        return {};
    std::filesystem::path path = PathFromUtf8(pref);
    SDL_free(pref);
    return path / name;
}

std::string ReadPref(const char* name)
{
    std::ifstream in(PrefFile(name));
    std::string line;
    std::getline(in, line);
    return line;
}

void WritePref(const char* name, const std::string& value)
{
    std::ofstream(PrefFile(name), std::ios::trunc) << value;
}

// --- Icons, drawn (no icon font) ----------------------------------------------------------------------------------------

enum class Icon
{
    Folder,
    FolderOpen,
    Archive,
    Image,
    Script,
    Sound,
    Model,
    Text,
    File,
};

Icon IconOf(std::string_view name)
{
    const std::string lower = Lower(name);
    const auto dot = lower.rfind('.');
    const std::string ext = dot == std::string::npos ? std::string() : lower.substr(dot + 1);
    for (const char* e : {"dds", "tga", "png", "jpg", "jpeg", "bmp"})
        if (ext == e)
            return Icon::Image;
    for (const char* e : {"lua", "kim", "fx", "fxo"})
        if (ext == e)
            return Icon::Script;
    for (const char* e : {"ogg", "wav", "mp3"})
        if (ext == e)
            return Icon::Sound;
    for (const char* e : {"x", "y", "xet", "skn", "skin"})
        if (ext == e)
            return Icon::Model;
    for (const char* e : {"txt", "xml", "ini", "tet", "tsv", "csv"})
        if (ext == e)
            return Icon::Text;
    return Icon::File;
}

ImU32 IconColor(Icon icon)
{
    switch (icon)
    {
    case Icon::Folder:
    case Icon::FolderOpen: return IM_COL32(222, 178, 90, 255);
    case Icon::Archive: return IM_COL32(178, 140, 240, 255);
    case Icon::Image: return IM_COL32(100, 190, 240, 255);
    case Icon::Script: return IM_COL32(140, 215, 140, 255);
    case Icon::Sound: return IM_COL32(240, 155, 90, 255);
    case Icon::Model: return IM_COL32(215, 140, 190, 255);
    case Icon::Text: return IM_COL32(190, 190, 190, 255);
    case Icon::File: break;
    }
    return IM_COL32(150, 150, 150, 255);
}

ImU32 Scaled(ImU32 color, float k, float alpha = 1.0f)
{
    ImVec4 c = ImGui::ColorConvertU32ToFloat4(color);
    return ImGui::ColorConvertFloat4ToU32(ImVec4(c.x * k, c.y * k, c.z * k, c.w * alpha));
}

void DrawIcon(ImDrawList* draw, ImVec2 p, float s, Icon icon, float alpha = 1.0f)
{
    const ImU32 color = Scaled(IconColor(icon), 1.0f, alpha), dark = Scaled(IconColor(icon), 0.6f, alpha);
    const auto at = [&](float x, float y) { return ImVec2(std::floor(p.x + x * s), std::floor(p.y + y * s)); };
    switch (icon)
    {
    case Icon::Folder:
        draw->AddRectFilled(at(0.06f, 0.18f), at(0.46f, 0.34f), color, 1.5f);
        draw->AddRectFilled(at(0.06f, 0.28f), at(0.94f, 0.84f), color, 1.5f);
        return;
    case Icon::FolderOpen:
        draw->AddRectFilled(at(0.06f, 0.18f), at(0.46f, 0.34f), dark, 1.5f);
        draw->AddRectFilled(at(0.06f, 0.28f), at(0.90f, 0.84f), dark, 1.5f);
        draw->AddQuadFilled(at(0.22f, 0.44f), at(1.00f, 0.44f), at(0.86f, 0.84f), at(0.06f, 0.84f), color);
        return;
    case Icon::Archive:
        draw->AddRectFilled(at(0.10f, 0.34f), at(0.90f, 0.88f), color, 1.5f);
        draw->AddRectFilled(at(0.04f, 0.16f), at(0.96f, 0.38f), dark, 1.5f);
        draw->AddRectFilled(at(0.38f, 0.46f), at(0.62f, 0.56f), dark, 1.0f);
        return;
    default: break;
    }
    // A page with a folded corner, in the type's color.
    const float c = 0.26f;
    const ImVec2 page[] = {at(0.18f, 0.08f), at(0.82f - c, 0.08f), at(0.82f, 0.08f + c), at(0.82f, 0.92f),
                           at(0.18f, 0.92f)};
    draw->AddConvexPolyFilled(page, 5, color);
    draw->AddTriangleFilled(at(0.82f - c, 0.08f), at(0.82f, 0.08f + c), at(0.82f - c, 0.08f + c), dark);
}

// An icon, then the rest of the line.
void IconHere(Icon icon)
{
    const float s = ImGui::GetFontSize();
    DrawIcon(ImGui::GetWindowDrawList(), ImGui::GetCursorScreenPos(), s, icon);
    ImGui::Dummy(ImVec2(s, s));
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
}

// --- State ------------------------------------------------------------------------------------------------------------

struct Folder
{
    std::string path;  // relative, '/'
    std::vector<std::size_t> children;  // folders
    std::vector<std::size_t> files;     // into ClientData::Files(), by name
    std::array<std::size_t, kStates> counts{};  // under it, recursive
    std::size_t total = 0;
};

// Work in the background (open, rescan, commit on a copy of the data, swapped in when done; pack).
struct Job
{
    std::thread thread;
    std::atomic<bool> done{false};
    std::atomic<std::size_t> progress{0}, total{0};
    std::string label;
    const char* unit = "files";
    std::unique_ptr<ClientData> result;  // null: the data stays as it is
    std::string error;
    std::string message;  // on success
    bool ok = false;
    bool clearsHistory = false;  // the manifest changed: undo steps no longer apply
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
};

// One step of undo: how to take it back and do it again, on the data as it is then.
struct Step
{
    std::string label;
    std::function<bool(ClientData&, std::string&)> undo, redo;
};

enum class View
{
    Folders,
    Archives,
};

enum class RowKind
{
    Folder,
    File,
    Archive,
    DeletedGroup,
};

struct Row
{
    RowKind kind = RowKind::File;
    std::size_t index = 0;  // folder, file or archive
    int depth = 0;
    std::string key;
};

// Item keys: "d:<folder path>", "f:<file name, lower case>", "a:<archive>".
std::string FolderKey(const std::string& path) { return "d:" + path; }
std::string FileKey(const std::string& name) { return "f:" + Lower(name); }
std::string ArchiveKey(const std::string& name) { return "a:" + name; }

enum class Pick
{
    ClientData,
    PackDestination,
    Export,
};

struct State
{
    std::filesystem::path root;
    std::unique_ptr<ClientData> data;
    std::unique_ptr<Job> job;
    unsigned generation = 0;  // bumped when `data` changes
    std::filesystem::path trash;  // this run's deleted files, under .kom/trash (undo restores them)

    // Explorer
    std::vector<Folder> folders;  // [0]: the root
    std::unordered_map<std::string, std::size_t> folderIndex;
    std::vector<std::size_t> deleted;
    std::vector<std::vector<std::size_t>> byArchive;  // ArchiveOf, per Archives(); then the unassigned
    std::vector<std::array<std::size_t, kStates>> archiveCounts;
    std::vector<std::string> archiveNames;
    unsigned archivesBuilt = ~0u;
    std::array<std::size_t, kStates> counts{};
    View view = View::Folders;
    char filter[128] = {};
    bool onlyChanges = false;
    std::set<std::string> open;  // folder paths, "#archive:<name>", "#deleted"
    std::vector<Row> rows;
    std::string rowsKey;
    bool rowsDirty = true;
    std::string hoverFolder;  // under the mouse, for files dropped from outside
    bool hoverValid = false;
    std::string scrollTo;     // a key to bring into view

    // Selection
    std::set<std::string> selection;
    std::vector<std::string> cut;

    // Inspector
    char moveTo[260] = {};
    char newArchive[64] = {};
    bool newArchiveAsked = false;
    std::vector<std::string> newArchiveFor;  // keys the new archive goes to
    char renameTo[260] = {};
    std::string renaming;  // key
    bool renameAsked = false;

    // Pack
    char packTo[1024] = {};

    // Undo
    std::vector<Step> undo, redo;

    std::string message;
    bool messageIsError = false;
    bool quitAsked = false;
    bool syncAsked = false;
    bool commitAsked = false;
    bool deleteAsked = false;
    std::vector<std::string> dropped;

    SDL_Window* window = nullptr;
    std::vector<std::string> exportNames;  // files to export once a destination is picked

    std::string pickFrom;  // the folder dialog's start, alive while it is open
    Pick picking = Pick::ClientData;
    std::mutex pickedMutex;  // the folder dialog answers from another thread
    std::optional<std::string> picked;

    void Say(std::string text, bool error = false)
    {
        message = std::move(text);
        messageIsError = error;
    }
    bool Busy() const { return job != nullptr; }
    std::size_t Pending() const
    {
        return counts[Index(FileState::Untracked)] + counts[Index(FileState::Deleted)] + (data ? data->EditCount() : 0);
    }
    std::size_t Uncommitted() const
    {
        return Pending() + counts[Index(FileState::Modified)] + counts[Index(FileState::Added)];
    }
};

void BuildTree(State& s)
{
    s.folders.assign(1, Folder{});
    s.folderIndex = {{"", 0}};
    s.deleted.clear();
    s.counts = {};
    const auto folderOf = [&](const std::string& path) {
        // Creates the chain of folders down to `path`.
        if (const auto it = s.folderIndex.find(path); it != s.folderIndex.end())
            return it->second;
        std::size_t at = 0;
        std::size_t start = 0;
        while (start < path.size())
        {
            std::size_t end = path.find('/', start);
            if (end == std::string::npos)
                end = path.size();
            const std::string sub = path.substr(0, end);
            auto it = s.folderIndex.find(sub);
            if (it == s.folderIndex.end())
            {
                Folder f;
                f.path = sub;
                s.folders.push_back(std::move(f));
                it = s.folderIndex.emplace(sub, s.folders.size() - 1).first;
                s.folders[at].children.push_back(it->second);
            }
            at = it->second;
            start = end + 1;
        }
        return at;
    };
    for (const std::string& folder : s.data->Folders())
        folderOf(folder);
    const auto& files = s.data->Files();
    std::vector<std::string> lower(files.size());
    for (std::size_t i = 0; i < files.size(); ++i)
    {
        const ClientDataFile& f = files[i];
        lower[i] = Lower(f.name);
        ++s.counts[Index(f.state)];
        if (f.state == FileState::Deleted)
        {
            s.deleted.push_back(i);
            continue;
        }
        s.folders[folderOf(f.folder)].files.push_back(i);
        std::string at = f.folder;  // counts up the chain
        for (;;)
        {
            Folder& folder = s.folders[s.folderIndex.at(at)];
            ++folder.counts[Index(f.state)];
            ++folder.total;
            if (at.empty())
                break;
            at = Parent(at);
        }
    }
    for (Folder& f : s.folders)
    {
        std::sort(f.children.begin(), f.children.end(), [&](std::size_t a, std::size_t b) {
            return Lower(LastPart(s.folders[a].path)) < Lower(LastPart(s.folders[b].path));
        });
        std::sort(f.files.begin(), f.files.end(), [&](std::size_t a, std::size_t b) { return lower[a] < lower[b]; });
    }
    std::sort(s.deleted.begin(), s.deleted.end(), [&](std::size_t a, std::size_t b) { return lower[a] < lower[b]; });
}

void BuildArchives(State& s)
{
    s.archiveNames = s.data->Archives();
    std::unordered_map<std::string, std::size_t> order;
    for (std::size_t a = 0; a < s.archiveNames.size(); ++a)
        order.emplace(s.archiveNames[a], a);
    s.byArchive.assign(s.archiveNames.size() + 1, {});
    s.archiveCounts.assign(s.archiveNames.size() + 1, {});
    const auto& files = s.data->Files();
    for (std::size_t i = 0; i < files.size(); ++i)
    {
        if (files[i].state == FileState::Ignored)
            continue;
        const auto it = order.find(s.data->ArchiveOf(files[i]));
        const std::size_t a = it == order.end() ? s.archiveNames.size() : it->second;
        s.byArchive[a].push_back(i);
        ++s.archiveCounts[a][Index(files[i].state)];
    }
    s.archivesBuilt = s.generation;
}

// The data changed in place (an operation) or was replaced (a job): rebuild what is shown.
void Changed(State& s)
{
    ++s.generation;
    BuildTree(s);
    s.rowsDirty = true;
}

std::string RowsKey(const State& s)
{
    return std::to_string(s.generation) + (s.onlyChanges ? "1" : "0") + (s.view == View::Folders ? "F" : "A") + s.filter;
}

void AddFolderRows(State& s, std::size_t index, int depth)
{
    const Folder& folder = s.folders[index];
    for (const std::size_t child : folder.children)
    {
        s.rows.push_back(Row{RowKind::Folder, child, depth, FolderKey(s.folders[child].path)});
        if (s.open.contains(s.folders[child].path))
            AddFolderRows(s, child, depth + 1);
    }
    for (const std::size_t file : folder.files)
        s.rows.push_back(Row{RowKind::File, file, depth, FileKey(s.data->Files()[file].name)});
}

void BuildRows(State& s)
{
    const std::string key = RowsKey(s);
    if (!s.rowsDirty && key == s.rowsKey)
        return;
    s.rowsDirty = false;
    s.rowsKey = key;
    s.rows.clear();
    if (!s.data)
        return;
    const auto& files = s.data->Files();
    if (s.onlyChanges || s.filter[0])
    {
        const std::string needle = Lower(s.filter);
        for (std::size_t i = 0; i < files.size(); ++i)
        {
            if (s.onlyChanges && (files[i].state == FileState::Unchanged || files[i].state == FileState::Ignored) &&
                !s.data->HasArchiveEdit(files[i]))
                continue;
            if (!needle.empty() && Lower(files[i].name).find(needle) == std::string::npos)
                continue;
            s.rows.push_back(Row{RowKind::File, i, 0, FileKey(files[i].name)});
        }
        return;
    }
    if (s.view == View::Archives)
    {
        if (s.archivesBuilt != s.generation)
            BuildArchives(s);
        for (std::size_t a = 0; a < s.byArchive.size(); ++a)
        {
            if (a == s.archiveNames.size() && s.byArchive[a].empty())
                continue;
            const std::string name = a < s.archiveNames.size() ? s.archiveNames[a] : "";
            s.rows.push_back(Row{RowKind::Archive, a, 0, ArchiveKey(name)});
            if (s.open.contains("#archive:" + name))
                for (const std::size_t i : s.byArchive[a])
                    s.rows.push_back(Row{RowKind::File, i, 1, FileKey(files[i].name)});
        }
        return;
    }
    if (!s.deleted.empty())
    {
        s.rows.push_back(Row{RowKind::DeletedGroup, 0, 0, "#deleted"});
        if (s.open.contains("#deleted"))
            for (const std::size_t i : s.deleted)
                s.rows.push_back(Row{RowKind::File, i, 1, FileKey(files[i].name)});
    }
    AddFolderRows(s, 0, 0);
}

Job& StartJob(State& s, std::string label, std::unique_ptr<ClientData> data,
              std::function<bool(ClientData*, Job&, std::string&)> work)
{
    auto job = std::make_unique<Job>();
    job->label = std::move(label);
    job->result = std::move(data);
    Job* j = job.get();
    job->thread = std::thread([j, work = std::move(work)] {
        j->ok = work(j->result.get(), *j, j->error);
        j->done = true;
        app::WakeEventLoop();
    });
    s.job = std::move(job);
    return *j;
}

ClientData::Progress ProgressOf(Job& job)
{
    return [&job](std::size_t done, std::size_t total) {
        job.progress = done;
        job.total = total;
        app::WakeEventLoop();
    };
}

void OpenFolder(State& s, const std::filesystem::path& root)
{
    if (s.Busy())
        return;
    if (!s.trash.empty())
    {
        std::error_code ec;
        std::filesystem::remove_all(s.trash, ec);
        s.trash.clear();
    }
    s.root = root;
    s.selection.clear();
    s.cut.clear();
    s.open.clear();
    Job& job = StartJob(s, "Opening " + PathToUtf8(root), std::make_unique<ClientData>(),
                        [root](ClientData* data, Job& job, std::string& error) {
                            return data->Open(root, 0, error, ProgressOf(job));
                        });
    job.clearsHistory = true;
}

void Rescan(State& s)
{
    if (s.Busy() || !s.data)
        return;
    StartJob(s, "Reading client_data", std::make_unique<ClientData>(*s.data),
             [](ClientData* data, Job& job, std::string& error) { return data->Rescan(0, error, ProgressOf(job)); });
}

void SyncNow(State& s)
{
    if (s.Busy() || !s.data)
        return;
    SyncStats stats;
    std::string error;
    if (!s.data->Sync(stats, error))
    {
        s.Say(error, true);
        return;
    }
    s.undo.clear();
    s.redo.clear();
    char text[200];
    std::snprintf(text, sizeof text,
                  "Manifest synced: %zu files added, %zu dropped, %zu moved to another archive, %zu archives created",
                  stats.added, stats.removed, stats.moved, stats.archives);
    s.Say(text);
    Changed(s);
}

void Commit(State& s)
{
    if (s.Busy() || !s.data)
        return;
    Job& job = StartJob(s, "Committing client_data", std::make_unique<ClientData>(*s.data),
                        [](ClientData* data, Job& job, std::string& error) {
                            ClientData::CommitStats stats;
                            if (!data->Commit(0, stats, error))
                                return false;
                            char text[240];
                            std::snprintf(text, sizeof text,
                                          "Committed: %zu files took their content as the original, %zu added, %zu "
                                          "dropped, %zu moved to another archive",
                                          stats.committed, stats.sync.added, stats.sync.removed, stats.sync.moved);
                            job.message = text;
                            return true;
                        });
    job.clearsHistory = true;
}

void Pack(State& s, bool onlyChanged)
{
    if (s.Busy() || !s.data || !s.packTo[0])
        return;
    WritePref("pack_destination.txt", s.packTo);
    const std::filesystem::path root = s.root, destination = PathFromUtf8(s.packTo);
    Job& job = StartJob(s, onlyChanged ? "Packing the changed archives" : "Packing every archive", nullptr,
                        [root, destination, onlyChanged](ClientData*, Job& job, std::string& error) {
                            PackOptions options;
                            options.clientData = root;
                            options.destination = destination;
                            options.onlyChanged = onlyChanged;
                            options.progress = ProgressOf(job);
                            PackStats stats;
                            if (!x2::assets::Pack(options, stats, error))
                                return false;
                            char text[300];
                            std::snprintf(text, sizeof text,
                                          "Packed into %s: %zu archives written, %zu unchanged left as they were",
                                          PathToUtf8(destination).c_str(), stats.archives - stats.skipped,
                                          stats.skipped);
                            job.message = text;
                            return true;
                        });
    job.unit = "archives";
}

void FinishJob(State& s)
{
    if (!s.job || !s.job->done)
        return;
    s.job->thread.join();
    std::unique_ptr<Job> job = std::move(s.job);
    std::fprintf(stderr, "x2_assets: %s: %.2f s\n", job->label.c_str(),
                 std::chrono::duration<double>(std::chrono::steady_clock::now() - job->start).count());
    if (!job->ok)
    {
        s.Say(job->error, true);
        return;
    }
    if (!job->message.empty())
        s.Say(job->message);
    else if (s.messageIsError)
        s.Say("");
    if (!job->result)
        return;
    if (job->clearsHistory)
    {
        s.undo.clear();
        s.redo.clear();
    }
    s.data = std::move(job->result);
    if (s.trash.empty())
    {
        char name[64];
        std::snprintf(name, sizeof name, "%lld",
                      static_cast<long long>(std::chrono::system_clock::now().time_since_epoch().count()));
        s.trash = s.root / ".kom" / "trash" / name;
    }
    Changed(s);
    WritePref("last_client_data.txt", PathToUtf8(s.root));
}

// --- Operations, each one undoable --------------------------------------------------------------------------------------

void Did(State& s, Step step)
{
    s.undo.push_back(std::move(step));
    s.redo.clear();
    if (s.undo.size() > 200)
        s.undo.erase(s.undo.begin());
    s.data->SaveHashCache();
    Changed(s);
}

void Undo(State& s)
{
    if (s.undo.empty() || s.Busy() || !s.data)
        return;
    Step step = std::move(s.undo.back());
    s.undo.pop_back();
    std::string error;
    if (!step.undo(*s.data, error))
        s.Say("Cannot undo " + step.label + ": " + error, true);
    else
    {
        s.Say("Undone: " + step.label);
        s.redo.push_back(std::move(step));
    }
    s.data->SaveHashCache();
    Changed(s);
}

void Redo(State& s)
{
    if (s.redo.empty() || s.Busy() || !s.data)
        return;
    Step step = std::move(s.redo.back());
    s.redo.pop_back();
    std::string error;
    if (!step.redo(*s.data, error))
        s.Say("Cannot redo " + step.label + ": " + error, true);
    else
    {
        s.Say("Redone: " + step.label);
        s.undo.push_back(std::move(step));
    }
    s.data->SaveHashCache();
    Changed(s);
}

// The selection's top items: folders whose parent is not selected too, files outside every selected folder.
std::vector<std::string> TopItems(const State& s, const std::vector<std::string>& keys)
{
    std::vector<std::string> folders;
    for (const std::string& k : keys)
        if (k.starts_with("d:"))
            folders.push_back(k.substr(2));
    const auto insideSelected = [&](const std::string& path) {
        for (const std::string& f : folders)
            if (path != f && (f.empty() || path.starts_with(f + "/")))
                return true;
        return false;
    };
    std::vector<std::string> top;
    for (const std::string& k : keys)
    {
        if (k.starts_with("d:"))
        {
            if (!insideSelected(k.substr(2)))
                top.push_back(k);
        }
        else if (k.starts_with("f:"))
        {
            const ClientDataFile* f = s.data->Find(k.substr(2));
            if (f && f->state != FileState::Deleted && !insideSelected(Join(f->folder, "x")))
                top.push_back(k);
        }
    }
    return top;
}

// A move done, to take back or do again.
struct Moved
{
    bool folder = false;
    std::string from;     // the file's old folder, or the folder's old parent
    std::string oldName;
    std::string to;       // its folder or parent now
    std::string newName;
};

bool MoveBack(ClientData& data, const std::vector<Moved>& moves, std::string& error)
{
    for (auto it = moves.rbegin(); it != moves.rend(); ++it)
    {
        const bool ok = it->folder ? data.MoveFolder(Join(it->to, it->newName), it->from, error, it->oldName)
                                   : data.MoveFile(it->newName, it->from, error, it->oldName);
        if (!ok)
            return false;
    }
    return true;
}

bool MoveAgain(ClientData& data, const std::vector<Moved>& moves, std::string& error)
{
    for (const Moved& m : moves)
    {
        const bool ok = m.folder ? data.MoveFolder(Join(m.from, m.oldName), m.to, error, m.newName)
                                 : data.MoveFile(m.oldName, m.to, error, m.newName);
        if (!ok)
            return false;
    }
    return true;
}

void MoveItems(State& s, const std::vector<std::string>& keys, const std::string& target)
{
    std::vector<Moved> moves;
    std::string error;
    for (const std::string& k : TopItems(s, keys))
    {
        Moved m;
        bool ok = true;
        if (k.starts_with("d:"))
        {
            const std::string path = k.substr(2);
            if (path.empty() || Parent(path) == target || target == path || target.starts_with(path + "/"))
                continue;
            m.folder = true;
            m.from = Parent(path);
            m.oldName = m.newName = LastPart(path);
            m.to = target;
            ok = s.data->MoveFolder(path, target, error);
        }
        else
        {
            const ClientDataFile* f = s.data->Find(k.substr(2));
            if (!f || f->folder == target)
                continue;
            m.from = f->folder;
            m.oldName = m.newName = f->name;
            m.to = target;
            ok = s.data->MoveFile(f->name, target, error);
        }
        if (!ok)
        {
            s.Say(error, true);
            break;
        }
        moves.push_back(std::move(m));
    }
    if (moves.empty())
        return;
    if (!s.messageIsError)
        s.Say("Moved " + std::to_string(moves.size()) + " items to " + (target.empty() ? "the root" : target));
    s.open.insert(target);
    for (const Moved& m : moves)  // moved folders stay selected under their new path
        if (m.folder && s.selection.erase(FolderKey(Join(m.from, m.oldName))))
            s.selection.insert(FolderKey(Join(m.to, m.newName)));
    Did(s, Step{"move of " + std::to_string(moves.size()) + " items",
                [moves](ClientData& d, std::string& e) { return MoveBack(d, moves, e); },
                [moves](ClientData& d, std::string& e) { return MoveAgain(d, moves, e); }});
}

void RenameItem(State& s, const std::string& key, const std::string& name)
{
    Moved m;
    m.newName = name;
    std::string error;
    bool ok = false;
    if (key.starts_with("d:"))
    {
        const std::string path = key.substr(2);
        m.folder = true;
        m.from = m.to = Parent(path);
        m.oldName = LastPart(path);
        if (m.oldName == name)
            return;
        ok = s.data->MoveFolder(path, m.to, error, name);
        if (ok)
        {
            s.selection = {FolderKey(Join(m.to, name))};
            if (s.open.erase(path))
                s.open.insert(Join(m.to, name));
        }
    }
    else if (const ClientDataFile* f = s.data->Find(key.substr(2)))
    {
        m.from = m.to = f->folder;
        m.oldName = f->name;
        if (m.oldName == name)
            return;
        ok = s.data->MoveFile(f->name, f->folder, error, name);
        if (ok)
            s.selection = {FileKey(name)};
    }
    if (!ok)
    {
        s.Say(error, true);
        return;
    }
    s.Say("Renamed " + m.oldName + " to " + name);
    Did(s, Step{"rename of " + m.oldName, [m](ClientData& d, std::string& e) { return MoveBack(d, {m}, e); },
                [m](ClientData& d, std::string& e) { return MoveAgain(d, {m}, e); }});
}

void DeleteItems(State& s, const std::vector<std::string>& keys)
{
    auto trashed = std::make_shared<std::vector<ClientData::Trashed>>();
    std::string error;
    for (const std::string& k : TopItems(s, keys))
    {
        ClientData::Trashed t;
        bool ok;
        if (k.starts_with("d:"))
            ok = !k.substr(2).empty() && s.data->Trash(k.substr(2), true, s.trash, t, error);
        else
        {
            const ClientDataFile* f = s.data->Find(k.substr(2));
            ok = f && s.data->Trash(Join(f->folder, f->name), false, s.trash, t, error);
        }
        if (!ok)
        {
            if (!error.empty())
                s.Say(error, true);
            continue;
        }
        trashed->push_back(std::move(t));
    }
    if (trashed->empty())
        return;
    s.selection.clear();
    if (!s.messageIsError)
        s.Say("Deleted " + std::to_string(trashed->size()) + " items (Ctrl+Z restores them while the window is open)");
    const std::filesystem::path trash = s.trash;
    Did(s, Step{"delete of " + std::to_string(trashed->size()) + " items",
                [trashed, trash](ClientData& d, std::string& e) {
                    for (auto it = trashed->rbegin(); it != trashed->rend(); ++it)
                        if (!d.Restore(*it, trash, e))
                            return false;
                    return true;
                },
                [trashed, trash](ClientData& d, std::string& e) {
                    for (ClientData::Trashed& t : *trashed)
                    {
                        ClientData::Trashed again;
                        if (!d.Trash(t.path, t.folder, trash, again, e))
                            return false;
                        t = std::move(again);
                    }
                    return true;
                }});
}

void NewFolder(State& s, const std::string& parent)
{
    std::string path;
    for (int n = 1;; ++n)
    {
        path = Join(parent, n == 1 ? std::string("New folder") : "New folder " + std::to_string(n));
        if (!s.data->Folders().contains(path))
            break;
    }
    std::string error;
    if (!s.data->CreateFolder(path, error))
    {
        s.Say(error, true);
        return;
    }
    s.open.insert(parent);
    s.selection = {FolderKey(path)};
    s.scrollTo = FolderKey(path);
    Did(s, Step{"new folder", [path](ClientData& d, std::string& e) { return d.RemoveEmptyFolder(path, e); },
                [path](ClientData& d, std::string& e) { return d.CreateFolder(path, e); }});
    s.renaming = FolderKey(path);
    std::snprintf(s.renameTo, sizeof s.renameTo, "%s", LastPart(path).c_str());
    s.renameAsked = true;
}

// An edit of the archives, undone by putting back what was there before.
void EditStep(State& s, const std::string& label, ClientData::Edits before)
{
    ClientData::Edits after = s.data->GetEdits();
    Did(s, Step{label,
                [before](ClientData& d, std::string&) {
                    d.SetEdits(before);
                    return true;
                },
                [after](ClientData& d, std::string&) {
                    d.SetEdits(after);
                    return true;
                }});
}

void IgnoreStep(State& s, const std::string& label, IgnoreList before)
{
    IgnoreList after = s.data->Ignores();
    Did(s, Step{label, [before](ClientData& d, std::string& e) { return d.SetIgnores(before, e); },
                [after](ClientData& d, std::string& e) { return d.SetIgnores(after, e); }});
}

void SetArchiveOf(State& s, const std::vector<std::string>& keys, const std::string& archive,
                  std::optional<ClientData::Edits> before = std::nullopt)
{
    if (!before)
        before = s.data->GetEdits();
    std::size_t count = 0;
    std::string error;
    for (const std::string& k : keys)
    {
        std::size_t n = 0;
        if (k.starts_with("d:"))
        {
            if (s.data->SetFolderArchive(k.substr(2), archive, n, error))
                count += n;
        }
        else if (k.starts_with("f:"))
        {
            const ClientDataFile* f = s.data->Find(k.substr(2));
            if (f && f->state != FileState::Deleted && f->state != FileState::Ignored &&
                s.data->ArchiveOf(*f) != archive && s.data->SetArchive(f->name, archive, error))
                ++count;
        }
    }
    if (!error.empty())
        s.Say(error, true);
    else
        s.Say(std::to_string(count) + " files will go to " + archive + " (applied by Sync data structure or Commit)");
    EditStep(s, "target archive " + archive, *before);
}

void IgnoreItems(State& s, const std::vector<std::string>& keys, bool ignore)
{
    IgnoreList before = s.data->Ignores();
    std::string error;
    std::size_t count = 0;
    for (const std::string& k : keys)
    {
        bool ok = false;
        if (k.starts_with("d:") && ignore)
            ok = s.data->IgnoreFolder(k.substr(2), error);
        else if (k.starts_with("f:"))
        {
            const ClientDataFile* f = s.data->Find(k.substr(2));
            if (f && f->state == (ignore ? FileState::Untracked : FileState::Ignored))
                ok = ignore ? s.data->Ignore(f->name, error) : s.data->Unignore(f->name, error);
        }
        count += ok;
    }
    if (!error.empty())
        s.Say(error, true);
    else
        s.Say(std::to_string(count) + (ignore ? " items ignored (.kom/ignore.txt)" : " files no longer ignored"));
    if (count)
        IgnoreStep(s, ignore ? "ignore" : "stop ignoring", std::move(before));
}

void AddDropped(State& s)
{
    if (s.dropped.empty() || !s.data || s.Busy())
        return;
    auto added = std::make_shared<std::vector<ClientData::Trashed>>();
    std::string error;
    for (const std::string& path : s.dropped)
    {
        const std::filesystem::path source = PathFromUtf8(path);
        const std::string name = PathToUtf8(source.filename());
        const std::string folder = s.hoverValid ? s.hoverFolder : s.data->FolderForNew(name);
        if (!s.data->AddFile(source, folder, error))
        {
            s.Say(error, true);
            continue;
        }
        ClientData::Trashed t;  // what undo takes out again
        t.path = Join(folder, name);
        added->push_back(std::move(t));
    }
    s.dropped.clear();
    if (added->empty())
        return;
    if (!s.messageIsError)
        s.Say(std::to_string(added->size()) + " files added (Sync data structure records them)");
    const std::filesystem::path trash = s.trash;
    Did(s, Step{"add of " + std::to_string(added->size()) + " files",
                [added, trash](ClientData& d, std::string& e) {
                    for (ClientData::Trashed& t : *added)
                    {
                        ClientData::Trashed out;
                        if (!d.Trash(t.path, false, trash, out, e))
                            return false;
                        t = std::move(out);
                    }
                    return true;
                },
                [added, trash](ClientData& d, std::string& e) {
                    for (const ClientData::Trashed& t : *added)
                        if (!d.Restore(t, trash, e))
                            return false;
                    return true;
                }});
}

// Where pasting goes: the folder selected, else the selected file's folder, else the root.
std::string TargetFolder(const State& s)
{
    if (s.selection.size() == 1)
    {
        const std::string& k = *s.selection.begin();
        if (k.starts_with("d:"))
            return k.substr(2);
        if (const ClientDataFile* f = k.starts_with("f:") ? s.data->Find(k.substr(2)) : nullptr;
            f && f->state != FileState::Deleted)
            return f->folder;
    }
    return {};
}

std::vector<std::string> Selected(const State& s) { return {s.selection.begin(), s.selection.end()}; }

void Paste(State& s, const std::string& target)
{
    if (s.cut.empty())
        return;
    const std::vector<std::string> cut = std::move(s.cut);
    s.cut.clear();
    MoveItems(s, cut, target);
}

void StartRename(State& s)
{
    if (s.selection.size() != 1)
        return;
    const std::string k = *s.selection.begin();
    if (k.starts_with("d:") && k.size() > 2)
        std::snprintf(s.renameTo, sizeof s.renameTo, "%s", LastPart(k.substr(2)).c_str());
    else if (const ClientDataFile* f = k.starts_with("f:") ? s.data->Find(k.substr(2)) : nullptr;
             f && f->state != FileState::Deleted)
        std::snprintf(s.renameTo, sizeof s.renameTo, "%s", f->name.c_str());
    else
        return;
    s.renaming = k;
    s.renameAsked = true;
}

void AskExport(State& s, const std::vector<std::string>& keys);  // after the folder dialog

// --- Explorer ---------------------------------------------------------------------------------------------------------

void CountsText(ImDrawList* draw, ImVec2& p, const std::array<std::size_t, kStates>& counts)
{
    const auto part = [&](FileState state, const char* sign) {
        if (const std::size_t n = counts[Index(state)])
        {
            char text[32];
            std::snprintf(text, sizeof text, "%s%zu", sign, n);
            draw->AddText(p, ImGui::ColorConvertFloat4ToU32(StateColor(state)), text);
            p.x += ImGui::CalcTextSize(text).x + ImGui::GetStyle().ItemSpacing.x;
        }
    };
    part(FileState::Untracked, "+");
    part(FileState::Added, "*");
    part(FileState::Modified, "~");
    part(FileState::Deleted, "-");
}

void ContextMenu(State& s, const Row& row)
{
    if (!ImGui::BeginPopupContextItem("##menu"))
        return;
    if (!s.selection.contains(row.key))
        s.selection = {row.key};
    const std::vector<std::string> keys = Selected(s);
    const bool folderRow = row.kind == RowKind::Folder;
    const bool fileRow = row.kind == RowKind::File && s.data->Files()[row.index].state != FileState::Deleted;
    const bool movable = fileRow || (folderRow && !s.folders[row.index].path.empty());
    const std::string here = folderRow ? s.folders[row.index].path : fileRow ? s.data->Files()[row.index].folder : "";
    if (ImGui::MenuItem("Cut", "Ctrl+X", false, movable))
        s.cut = keys;
    if (ImGui::MenuItem(folderRow ? "Paste into this folder" : "Paste here", "Ctrl+V", false,
                        !s.cut.empty() && (folderRow || fileRow)))
        Paste(s, here);
    ImGui::Separator();
    if (ImGui::MenuItem("Rename", "F2", false, movable && keys.size() == 1))
        StartRename(s);
    if (ImGui::MenuItem("Delete", "Del", false, movable))
        s.deleteAsked = true;
    if (ImGui::MenuItem("Export...", nullptr, false, folderRow || fileRow))
        AskExport(s, keys);
    ImGui::SetItemTooltip("Copies the selected files (folders: every file in them) to a folder you choose, in "
                          "client_data's folders");
    if (ImGui::MenuItem("New folder", nullptr, false, folderRow || fileRow))
        NewFolder(s, here);
    ImGui::Separator();
    if (ImGui::BeginMenu("Target archive", folderRow || fileRow))
    {
        if (ImGui::MenuItem("New archive..."))
        {
            s.newArchiveFor = keys;
            s.newArchiveAsked = true;
        }
        ImGui::Separator();
        for (const std::string& archive : s.data->Archives())
            if (ImGui::MenuItem(archive.c_str()))
                SetArchiveOf(s, keys, archive);
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Ignore", nullptr, false, movable))
        IgnoreItems(s, keys, true);
    if (ImGui::MenuItem("Stop ignoring", nullptr, false, fileRow))
        IgnoreItems(s, keys, false);
    ImGui::EndPopup();
}

std::string OpenKey(const State& s, const Row& row)
{
    switch (row.kind)
    {
    case RowKind::Folder: return s.folders[row.index].path;
    case RowKind::Archive:
        return "#archive:" + (row.index < s.archiveNames.size() ? s.archiveNames[row.index] : std::string());
    case RowKind::DeletedGroup: return "#deleted";
    case RowKind::File: break;
    }
    return {};
}

void DrawRow(State& s, const Row& row, int rowIndex)
{
    const auto& files = s.data->Files();
    const float font = ImGui::GetFontSize();
    const float indent = font * 1.1f;
    const bool selected = s.selection.contains(row.key);
    const bool isCut = std::find(s.cut.begin(), s.cut.end(), row.key) != s.cut.end();
    const bool fileRow = row.kind == RowKind::File && files[row.index].state != FileState::Deleted;

    ImGui::PushID(rowIndex);
    ImGui::SetNextItemSelectionUserData(rowIndex);
    ImGui::Selectable("##row", selected);
    // A click on a folder (no Ctrl/Shift) opens or closes it, as in VS Code.
    const ImGuiIO& io = ImGui::GetIO();
    if (row.kind != RowKind::File && ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
        !io.KeyCtrl && !io.KeyShift &&
        io.MouseDragMaxDistanceSqr[ImGuiMouseButton_Left] < io.MouseDragThreshold * io.MouseDragThreshold)
    {
        const std::string key = OpenKey(s, row);
        if (!s.open.erase(key))
            s.open.insert(key);
        s.rowsDirty = true;
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
    {
        s.hoverValid = row.kind == RowKind::Folder || fileRow;
        s.hoverFolder = row.kind == RowKind::Folder ? s.folders[row.index].path
                        : fileRow                   ? files[row.index].folder
                                                    : std::string();
    }
    if (!s.scrollTo.empty() && s.scrollTo == row.key)
    {
        ImGui::SetScrollHereY(0.4f);
        s.scrollTo.clear();
    }

    // Drag the selection onto a folder (moves it there) or an archive (makes it their target).
    const bool draggable = fileRow || (row.kind == RowKind::Folder && !s.folders[row.index].path.empty());
    if (draggable && ImGui::BeginDragDropSource())
    {
        if (!s.selection.contains(row.key))
            s.selection = {row.key};
        const int dummy = 0;
        ImGui::SetDragDropPayload("X2_ITEMS", &dummy, sizeof dummy);
        ImGui::Text("Move %zu items", s.selection.size());
        ImGui::EndDragDropSource();
    }
    if ((row.kind == RowKind::Folder || (row.kind == RowKind::Archive && row.index < s.archiveNames.size())) &&
        ImGui::BeginDragDropTarget())
    {
        if (ImGui::AcceptDragDropPayload("X2_ITEMS"))
        {
            if (row.kind == RowKind::Folder)
                MoveItems(s, Selected(s), s.folders[row.index].path);
            else
                SetArchiveOf(s, Selected(s), s.archiveNames[row.index]);
        }
        ImGui::EndDragDropTarget();
    }
    ContextMenu(s, row);

    // The row's content, drawn over the selectable.
    ImDrawList* draw = ImGui::GetWindowDrawList();
    const ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 p(min.x + static_cast<float>(row.depth) * indent, min.y);
    const float alpha = isCut ? 0.45f : 1.0f;
    const auto text = [&](const std::string& t, ImVec4 color) {
        color.w *= alpha;
        draw->AddText(p, ImGui::ColorConvertFloat4ToU32(color), t.c_str());
        p.x += ImGui::CalcTextSize(t.c_str()).x + ImGui::GetStyle().ItemSpacing.x;
    };
    const auto icon = [&](Icon i) {
        DrawIcon(draw, p, font, i, alpha);
        p.x += font + ImGui::GetStyle().ItemInnerSpacing.x;
    };
    const ImVec4 dim = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
    switch (row.kind)
    {
    case RowKind::Folder:
    {
        const Folder& folder = s.folders[row.index];
        icon(s.open.contains(folder.path) ? Icon::FolderOpen : Icon::Folder);
        const bool ignored = folder.total && folder.counts[Index(FileState::Ignored)] == folder.total;
        const FileState state = ignored                                     ? FileState::Ignored
                                : folder.counts[Index(FileState::Modified)]  ? FileState::Modified
                                : folder.counts[Index(FileState::Untracked)] ? FileState::Untracked
                                                                             : FileState::Unchanged;
        text(LastPart(folder.path), StateColor(state));
        text(std::to_string(folder.total), dim);
        CountsText(draw, p, folder.counts);
        break;
    }
    case RowKind::File:
    {
        const ClientDataFile& f = files[row.index];
        icon(IconOf(f.name));
        text(f.name, StateColor(f.state));
        if (s.onlyChanges || s.filter[0])
            text(f.state == FileState::Deleted ? "(deleted)" : f.folder.empty() ? "(root)" : f.folder, dim);
        if (s.data->HasArchiveEdit(f))
            text("-> " + s.data->ArchiveOf(f), StateColor(FileState::Modified));
        break;
    }
    case RowKind::Archive:
    {
        icon(Icon::Archive);
        const bool known = row.index < s.archiveNames.size();
        text(known ? s.archiveNames[row.index] : "(no archive)", ImGui::GetStyleColorVec4(ImGuiCol_Text));
        text(std::to_string(s.byArchive[row.index].size()), dim);
        CountsText(draw, p, s.archiveCounts[row.index]);
        break;
    }
    case RowKind::DeletedGroup:
        icon(s.open.contains("#deleted") ? Icon::FolderOpen : Icon::Folder);
        text("Deleted", StateColor(FileState::Deleted));
        text(std::to_string(s.deleted.size()), dim);
        break;
    }
    ImGui::PopID();
}

void ApplyRequests(State& s, ImGuiMultiSelectIO* io)
{
    for (const ImGuiSelectionRequest& r : io->Requests)
    {
        if (r.Type == ImGuiSelectionRequestType_SetAll)
        {
            s.selection.clear();
            if (r.Selected)
                for (const Row& row : s.rows)
                    if (row.kind != RowKind::DeletedGroup)
                        s.selection.insert(row.key);
        }
        else if (r.Type == ImGuiSelectionRequestType_SetRange)
        {
            const auto last = std::min<ImGuiSelectionUserData>(r.RangeLastItem,
                                                                static_cast<ImGuiSelectionUserData>(s.rows.size()) - 1);
            for (auto i = r.RangeFirstItem; i <= last; ++i)
            {
                const Row& row = s.rows[static_cast<std::size_t>(i)];
                if (row.kind == RowKind::DeletedGroup)
                    continue;
                if (r.Selected)
                    s.selection.insert(row.key);
                else
                    s.selection.erase(row.key);
            }
        }
    }
}

void Explorer(State& s)
{
    if (ImGui::RadioButton("Folders", s.view == View::Folders))
        s.view = View::Folders;
    ImGui::SameLine();
    if (ImGui::RadioButton("Archives", s.view == View::Archives))
        s.view = View::Archives;
    ImGui::SameLine();
    ImGui::Checkbox("Only changes", &s.onlyChanges);
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::InputTextWithHint("##filter", "Find a file by name", s.filter, sizeof s.filter);
    ImGui::Separator();
    if (!s.data)
        return;
    BuildRows(s);

    ImGui::BeginChild("rows");
    s.hoverValid = false;
    ImGuiMultiSelectIO* io =
        ImGui::BeginMultiSelect(ImGuiMultiSelectFlags_BoxSelect1d | ImGuiMultiSelectFlags_ClearOnEscape |
                                    ImGuiMultiSelectFlags_ClearOnClickVoid,
                                static_cast<int>(s.selection.size()), static_cast<int>(s.rows.size()));
    ApplyRequests(s, io);
    const std::vector<Row> rows = s.rows;  // an operation in a row may rebuild s.rows
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(rows.size()));
    if (io->RangeSrcItem != -1)
        clipper.IncludeItemByIndex(static_cast<int>(io->RangeSrcItem));
    if (!s.scrollTo.empty())
        for (std::size_t i = 0; i < rows.size(); ++i)
            if (rows[i].key == s.scrollTo)
                clipper.IncludeItemByIndex(static_cast<int>(i));
    while (clipper.Step())
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            DrawRow(s, rows[static_cast<std::size_t>(i)], i);
    io = ImGui::EndMultiSelect();
    if (rows.size() == s.rows.size())
        ApplyRequests(s, io);

    // Empty space: the root.
    if (ImGui::BeginPopupContextWindow("##void", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
    {
        if (ImGui::MenuItem("Paste here (root)", "Ctrl+V", false, !s.cut.empty()))
            Paste(s, "");
        if (ImGui::MenuItem("New folder"))
            NewFolder(s, "");
        ImGui::EndPopup();
    }
    if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered())
    {
        s.hoverValid = true;
        s.hoverFolder.clear();
    }

    if (ImGui::IsWindowFocused() && !ImGui::GetIO().WantTextInput && !s.Busy())
    {
        if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_X) && !s.selection.empty())
        {
            s.cut = Selected(s);
            s.Say(std::to_string(s.cut.size()) + " items cut: Ctrl+V on a folder moves them there");
        }
        if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_V))
            Paste(s, TargetFolder(s));
        if (ImGui::Shortcut(ImGuiKey_Delete) && !s.selection.empty())
            s.deleteAsked = true;
        if (ImGui::Shortcut(ImGuiKey_F2))
            StartRename(s);
        if (!s.cut.empty() && ImGui::IsKeyPressed(ImGuiKey_Escape))
            s.cut.clear();
    }
    ImGui::EndChild();
}

// --- Inspector --------------------------------------------------------------------------------------------------------

constexpr int kNewArchive = -2;

// The archives in load order, then "New archive...". `chosen`: an index into Archives(), or kNewArchive.
bool ArchiveCombo(const char* label, const std::vector<std::string>& archives, const std::string& current, int& chosen)
{
    bool picked = false;
    if (ImGui::BeginCombo(label, current.empty() ? "(choose)" : current.c_str(), ImGuiComboFlags_HeightLarge))
    {
        if (ImGui::Selectable("New archive..."))
        {
            chosen = kNewArchive;
            picked = true;
        }
        ImGui::Separator();
        for (std::size_t a = 0; a < archives.size(); ++a)
        {
            if (ImGui::Selectable(archives[a].c_str(), archives[a] == current))
            {
                chosen = static_cast<int>(a);
                picked = true;
            }
        }
        ImGui::EndCombo();
    }
    return picked;
}

void TargetArchive(State& s, const std::vector<std::string>& keys, const std::string& current)
{
    const std::vector<std::string> archives = s.data->Archives();
    int chosen = -1;
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ArchiveCombo("##archive", archives, current, chosen))
    {
        if (chosen == kNewArchive)
        {
            s.newArchiveFor = keys;
            s.newArchiveAsked = true;
        }
        else
            SetArchiveOf(s, keys, archives[static_cast<std::size_t>(chosen)]);
    }
}

void Field(const char* label, const std::string& value)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TextDisabled("%s", label);
    ImGui::TableNextColumn();
    ImGui::TextWrapped("%s", value.c_str());
}

void MoveBox(State& s)
{
    ImGui::SeparatorText("Move");
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::InputTextWithHint("##move", "Folder to move to (e.g. ui/textures), or drag, or Ctrl+X / Ctrl+V", s.moveTo,
                             sizeof s.moveTo);
    ImGui::BeginDisabled(!s.moveTo[0]);
    if (ImGui::Button("Move"))
        MoveItems(s, Selected(s), s.moveTo);
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button("Rename... (F2)"))
        StartRename(s);
    ImGui::SameLine();
    if (ImGui::Button("Delete... (Del)"))
        s.deleteAsked = true;
    ImGui::SameLine();
    if (ImGui::Button("Export..."))
        AskExport(s, Selected(s));
    ImGui::SetItemTooltip("Copies the selection to a folder you choose, in client_data's folders");
}

void FileInspector(State& s, const ClientDataFile& f)
{
    IconHere(IconOf(f.name));
    ImGui::TextUnformatted(f.name.c_str());
    ImGui::TextColored(StateColor(f.state), "%s", StateLabel(f.state));
    ImGui::Separator();
    if (ImGui::BeginTable("file", 2, ImGuiTableFlags_SizingStretchProp))
    {
        Field("Folder", f.state == FileState::Deleted ? "(not in client_data)" : f.folder.empty() ? "(root)" : f.folder);
        if (f.state != FileState::Deleted)
        {
            Field("Size", FormatSize(f.size));
            Field("SHA-256", f.sha256);
        }
        if (f.record >= 0)
        {
            const FileRecord& r = s.data->GetManifest().files[static_cast<std::size_t>(f.record)];
            Field("Record", r.name);
            Field("Archive", r.archive + ", entry " + std::to_string(r.index));
            Field("Algorithm", std::to_string(r.algorithm));
            Field("Encoding", std::string(ToString(r.encoding)));
            Field("Form", std::string(ToString(r.form)));
            if (r.encoding != Encoding::New)
                Field("Original", FormatSize(r.size) + " (" + FormatSize(r.storedSize) + " stored)");
        }
        ImGui::EndTable();
    }
    if (f.state == FileState::Deleted)
        return;
    const std::vector<std::string> keys = Selected(s);
    if (f.state == FileState::Ignored)
    {
        ImGui::SeparatorText("Ignored");
        if (ImGui::Button("Stop ignoring"))
            IgnoreItems(s, keys, false);
    }
    else
    {
        ImGui::SeparatorText("Target archive");
        TargetArchive(s, keys, s.data->ArchiveOf(f));
        if (s.data->HasArchiveEdit(f))
        {
            ImGui::TextColored(StateColor(FileState::Modified), "Edited: applied by Sync data structure or Commit");
            if (ImGui::Button("Revert"))
            {
                const std::string base = f.record >= 0
                                             ? s.data->GetManifest().files[static_cast<std::size_t>(f.record)].archive
                                             : s.data->DefaultArchive(f.folder);
                SetArchiveOf(s, keys, base);
            }
        }
        else if (f.record < 0)
            ImGui::TextDisabled("The folder's default (its most common archive)");
        if (f.state == FileState::Untracked && ImGui::Button("Ignore (never sync, commit or pack it)"))
            IgnoreItems(s, keys, true);
    }
    MoveBox(s);
}

void FolderInspector(State& s, const std::string& path)
{
    const auto it = s.folderIndex.find(path);
    if (it == s.folderIndex.end())
        return;
    const Folder& folder = s.folders[it->second];
    IconHere(Icon::Folder);
    ImGui::TextUnformatted(path.empty() ? "(root)" : path.c_str());
    ImGui::TextDisabled("Folder, %zu files", folder.total);
    ImGui::Separator();
    if (ImGui::BeginTable("folder", 2, ImGuiTableFlags_SizingStretchProp))
    {
        for (const FileState st : {FileState::Unchanged, FileState::Modified, FileState::Untracked, FileState::Added,
                                   FileState::Ignored})
            if (folder.counts[Index(st)])
                Field(std::string(ToString(st)).c_str(), std::to_string(folder.counts[Index(st)]));
        const std::string def = s.data->DefaultArchive(path);
        Field("New files go to", def.empty() ? "(no archive)" : def);
        ImGui::EndTable();
    }
    ImGui::SeparatorText("Target archive of every file in it");
    TargetArchive(s, Selected(s), "");
    if (!path.empty())
    {
        ImGui::SeparatorText("Ignore");
        if (ImGui::Button("Ignore this folder (its new files are never synced, committed or packed)"))
            IgnoreItems(s, Selected(s), true);
        MoveBox(s);
    }
    ImGui::Spacing();
    ImGui::TextDisabled("Drop files from outside on a folder to add them there.");
}

void ArchiveInspector(State& s, const std::string& name)
{
    IconHere(Icon::Archive);
    ImGui::TextUnformatted(name.empty() ? "(no archive)" : name.c_str());
    if (s.archivesBuilt != s.generation)
        BuildArchives(s);
    for (std::size_t a = 0; a < s.archiveNames.size(); ++a)
    {
        if (s.archiveNames[a] != name)
            continue;
        ImGui::TextDisabled("Archive %zu of %zu in load order, %zu files after a sync", a + 1, s.archiveNames.size(),
                            s.byArchive[a].size());
        const auto& manifest = s.data->GetManifest().archives;
        if (std::none_of(manifest.begin(), manifest.end(), [&](const ArchiveRecord& r) { return r.name == name; }))
            ImGui::TextColored(StateColor(FileState::Untracked), "New: created by the next sync or commit");
    }
    ImGui::Spacing();
    ImGui::TextDisabled("Drag files onto an archive to make it their target.");
    ImGui::TextDisabled("The client loads only its own list of archives (data001-150, datai001-023).");
}

void MultiInspector(State& s)
{
    std::size_t files = 0, folders = 0;
    for (const std::string& k : s.selection)
        (k.starts_with("d:") ? folders : files) += 1;
    ImGui::Text("%zu items selected: %zu files, %zu folders", s.selection.size(), files, folders);
    ImGui::Separator();
    ImGui::SeparatorText("Target archive of all of them (folders: every file in them)");
    TargetArchive(s, Selected(s), "");
    ImGui::SeparatorText("Ignore");
    if (ImGui::Button("Ignore the new files and folders"))
        IgnoreItems(s, Selected(s), true);
    ImGui::SameLine();
    if (ImGui::Button("Stop ignoring"))
        IgnoreItems(s, Selected(s), false);
    MoveBox(s);
}

void Inspector(State& s)
{
    if (!s.data)
        return;
    if (s.selection.empty())
    {
        ImGui::TextDisabled("Select a file, a folder or an archive (Ctrl or Shift for more, or drag a box).");
        ImGui::Spacing();
        ImGui::TextDisabled("Ctrl+X / Ctrl+V move, F2 renames, Del deletes, Ctrl+Z / Ctrl+Y undo and redo,");
        ImGui::TextDisabled("drag onto a folder to move or onto an archive to set the target, right-click for more.");
        return;
    }
    if (s.selection.size() > 1)
    {
        MultiInspector(s);
        return;
    }
    const std::string key = *s.selection.begin();
    if (key.starts_with("f:"))
    {
        if (const ClientDataFile* f = s.data->Find(key.substr(2)))
            FileInspector(s, *f);
    }
    else if (key.starts_with("d:"))
        FolderInspector(s, key.substr(2));
    else if (key.starts_with("a:"))
        ArchiveInspector(s, key.substr(2));
}

// --- Toolbar and popups -----------------------------------------------------------------------------------------------

void PickFolder(State& s, SDL_Window* window, Pick what)
{
    s.picking = what;
    s.pickFrom = what == Pick::ClientData        ? PathToUtf8(s.root)
                 : what == Pick::PackDestination ? std::string(s.packTo)
                                                 : ReadPref("export_destination.txt");
    SDL_ShowOpenFolderDialog(
        [](void* user, const char* const* list, int) {
            auto* state = static_cast<State*>(user);
            if (list && list[0])
            {
                const std::lock_guard lock(state->pickedMutex);
                state->picked = list[0];
            }
            app::WakeEventLoop();
        },
        &s, window, s.pickFrom.empty() ? nullptr : s.pickFrom.c_str(), false);
}

// The files of the selected items (a folder: every file under it), to export.
std::vector<std::string> ExportNames(const State& s, const std::vector<std::string>& keys)
{
    std::vector<std::string> names;
    for (const std::string& k : keys)
    {
        if (k.starts_with("d:"))
        {
            const std::vector<std::string> under = s.data->FilesUnder(k.substr(2));
            names.insert(names.end(), under.begin(), under.end());
        }
        else if (const ClientDataFile* f = k.starts_with("f:") ? s.data->Find(k.substr(2)) : nullptr;
                 f && f->state != FileState::Deleted)
            names.push_back(f->name);
    }
    return names;
}

void AskExport(State& s, const std::vector<std::string>& keys)
{
    s.exportNames = ExportNames(s, keys);
    if (s.exportNames.empty())
    {
        s.Say("Nothing to export: the selection holds no file in client_data", true);
        return;
    }
    PickFolder(s, s.window, Pick::Export);
}

void ExportTo(State& s, const std::filesystem::path& destination)
{
    if (s.Busy() || !s.data || s.exportNames.empty())
        return;
    WritePref("export_destination.txt", PathToUtf8(destination));
    const ClientData* data = s.data.get();  // read only; the overlay keeps the window from changing it
    Job& job = StartJob(s, "Exporting " + std::to_string(s.exportNames.size()) + " files", nullptr,
                        [data, destination, names = std::move(s.exportNames)](ClientData*, Job& job,
                                                                              std::string& error) {
                            std::size_t copied = 0;
                            if (!data->Export(names, destination, 0, copied, error, ProgressOf(job)))
                                return false;
                            job.message = std::to_string(copied) + " files exported to " + PathToUtf8(destination) +
                                          ", in client_data's folders";
                            return true;
                        });
    s.exportNames.clear();
    (void)job;
}

void Toolbar(State& s, SDL_Window* window)
{
    if (ImGui::Button("Open..."))
        PickFolder(s, window, Pick::ClientData);
    ImGui::SameLine();
    if (ImGui::Button("Rescan (F5)"))
        Rescan(s);
    ImGui::SetItemTooltip("Reads client_data again, for changes made outside (only what changed is hashed again).");
    ImGui::SameLine();
    ImGui::BeginDisabled(s.undo.empty());
    if (ImGui::ArrowButton("##undo", ImGuiDir_Left))
        Undo(s);
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Undo (Ctrl+Z)%s%s", s.undo.empty() ? "" : ": ", s.undo.empty() ? "" : s.undo.back().label.c_str());
    ImGui::SameLine();
    ImGui::BeginDisabled(s.redo.empty());
    if (ImGui::ArrowButton("##redo", ImGuiDir_Right))
        Redo(s);
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Redo (Ctrl+Y)%s%s", s.redo.empty() ? "" : ": ", s.redo.empty() ? "" : s.redo.back().label.c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("|");
    ImGui::SameLine();
    const std::size_t edits = s.data ? s.data->EditCount() : 0;
    ImGui::BeginDisabled(!s.data || s.Pending() == 0);
    if (ImGui::Button("Sync data structure"))
        s.syncAsked = true;
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Writes the manifest's structure: adds the %zu new files, drops the %zu deleted ones and "
                          "applies %zu edits (archives).\nModified files stay modified: the manifest keeps their "
                          "original.",
                          s.counts[Index(FileState::Untracked)], s.counts[Index(FileState::Deleted)], edits);
    ImGui::SameLine();
    ImGui::BeginDisabled(!s.data || s.Uncommitted() == 0);
    if (ImGui::Button("Commit"))
        s.commitAsked = true;
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Makes client_data's content the original: a sync, then modified and new files become "
                          "originals.\nAfterwards nothing shows as changed; Pack writes them into the .kom.");
    ImGui::SameLine();
    ImGui::TextDisabled("|");
    ImGui::SameLine();
    ImGui::BeginDisabled(!s.data || !s.packTo[0]);
    if (ImGui::Button("Pack"))
        Pack(s, true);
    ImGui::EndDisabled();
    if (ImGui::BeginPopupContextItem("##packAll"))
    {
        if (ImGui::MenuItem("Pack every archive"))
            Pack(s, false);
        ImGui::EndPopup();
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Writes the .kom of the manifest into the destination: only the archives that changed since "
                          "the last pack there.\nRight-click: pack every archive. Pack does not change the manifest "
                          "(that is Commit).");
    ImGui::SameLine();
    ImGui::TextUnformatted("to");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(std::max(200.0f, ImGui::GetContentRegionAvail().x - ImGui::GetFrameHeight() * 2));
    ImGui::InputTextWithHint("##packTo", ".kom destination folder (e.g. the client's data folder)", s.packTo,
                             sizeof s.packTo);
    if (ImGui::IsItemDeactivatedAfterEdit())
        WritePref("pack_destination.txt", s.packTo);
    ImGui::SameLine();
    if (ImGui::Button("..."))
        PickFolder(s, window, Pick::PackDestination);
    ImGui::SetItemTooltip("Choose the .kom destination folder");

    ImGui::TextDisabled("%s", s.root.empty() ? "No client_data open" : PathToUtf8(s.root).c_str());
    if (s.data)
    {
        const auto count = [&](FileState state, const char* what) {
            ImGui::SameLine();
            ImGui::TextColored(StateColor(state), "%zu %s", s.counts[Index(state)], what);
        };
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        count(FileState::Untracked, "new");
        count(FileState::Modified, "modified");
        count(FileState::Deleted, "deleted");
        count(FileState::Added, "added");
        count(FileState::Ignored, "ignored");
        if (edits)
        {
            ImGui::SameLine();
            ImGui::TextColored(StateColor(FileState::Modified), "| %zu unsynced edits", edits);
        }
    }
}

void StatusBar(State& s)
{
    if (s.message.empty())
        return;
    if (s.messageIsError)
        ImGui::TextColored(StateColor(FileState::Deleted), "%s", s.message.c_str());
    else
        ImGui::TextUnformatted(s.message.c_str());
}

bool Confirm(const char* name, const std::function<void()>& body, const char* yes)
{
    bool answered = false;
    if (ImGui::BeginPopupModal(name, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        body();
        if (ImGui::Button(yes) || ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))
        {
            answered = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    return answered;
}

// True when the window should close.
bool Popups(State& s)
{
    // Work in the background blocks the window: what it reads or writes must not change under it.
    if (s.job && !ImGui::IsPopupOpen("##busy"))
        ImGui::OpenPopup("##busy");
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(std::min(560.0f, viewport->WorkSize.x * 0.8f), 0));
    if (ImGui::BeginPopupModal("##busy", nullptr,
                               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (!s.job)
            ImGui::CloseCurrentPopup();
        else
        {
            const std::size_t total = s.job->total, done = s.job->progress;
            ImGui::TextUnformatted(s.job->label.c_str());
            char overlay[96];
            if (total)
                std::snprintf(overlay, sizeof overlay, "%zu / %zu %s", done, total, s.job->unit);
            else
                std::snprintf(overlay, sizeof overlay, "listing files...");
            ImGui::ProgressBar(total ? static_cast<float>(done) / static_cast<float>(total)
                                     : -1.0f * static_cast<float>(ImGui::GetTime()),
                               ImVec2(-FLT_MIN, 0), overlay);
        }
        ImGui::EndPopup();
    }
    if (s.job)
        return false;

    if (s.syncAsked)
    {
        s.syncAsked = false;
        if (s.counts[Index(FileState::Deleted)] == 0)
            SyncNow(s);
        else
            ImGui::OpenPopup("Sync data structure");
    }
    if (Confirm(
            "Sync data structure",
            [&] {
                ImGui::Text("%zu deleted files will be dropped from the manifest: their original entries leave the "
                            "archives.",
                            s.counts[Index(FileState::Deleted)]);
                ImGui::TextDisabled("The previous manifest is kept as .kom/manifest.tsv.bak. Undo history is cleared.");
            },
            "Sync"))
        SyncNow(s);

    if (s.commitAsked)
    {
        s.commitAsked = false;
        ImGui::OpenPopup("Commit");
    }
    if (Confirm(
            "Commit",
            [&] {
                ImGui::TextUnformatted("client_data's content becomes the original (the .kom are the source of truth):");
                ImGui::BulletText("%zu modified and %zu added files become originals",
                                  s.counts[Index(FileState::Modified)], s.counts[Index(FileState::Added)]);
                ImGui::BulletText("%zu new files are added, %zu deleted ones dropped, %zu edits applied",
                                  s.counts[Index(FileState::Untracked)], s.counts[Index(FileState::Deleted)],
                                  s.data ? s.data->EditCount() : 0);
                ImGui::TextDisabled("Nothing is packed: Pack writes the .kom afterwards.");
                ImGui::TextDisabled("The previous manifest is kept as .kom/manifest.tsv.bak. Undo history is cleared.");
            },
            "Commit"))
        Commit(s);

    if (s.newArchiveAsked)
    {
        s.newArchiveAsked = false;
        s.newArchive[0] = 0;
        ImGui::OpenPopup("New archive");
    }
    if (Confirm(
            "New archive",
            [&] {
                ImGui::TextUnformatted("A new .kom, created by the next sync or commit; its place in the load order is "
                                       "its name's.");
                ImGui::TextDisabled("The client loads only its own list (data001-150, datai001-023).");
                if (ImGui::IsWindowAppearing())
                    ImGui::SetKeyboardFocusHere();
                ImGui::InputTextWithHint("##name", "e.g. data151.kom", s.newArchive, sizeof s.newArchive);
            },
            "Create"))
    {
        std::string error;
        std::string name = s.newArchive;
        if (!Lower(name).ends_with(".kom"))
            name += ".kom";
        const ClientData::Edits before = s.data->GetEdits();
        if (!s.data->CreateArchive(name, error))
            s.Say(error, true);
        else
            SetArchiveOf(s, s.newArchiveFor, name, before);
    }

    if (s.renameAsked)
    {
        s.renameAsked = false;
        ImGui::OpenPopup("Rename");
    }
    if (Confirm(
            "Rename",
            [&] {
                if (ImGui::IsWindowAppearing())
                    ImGui::SetKeyboardFocusHere();
                ImGui::SetNextItemWidth(400);
                ImGui::InputText("##rename", s.renameTo, sizeof s.renameTo);
                if (s.renaming.starts_with("f:"))
                    ImGui::TextDisabled("The game finds files by name: renaming one breaks what loads it.");
            },
            "Rename"))
        RenameItem(s, s.renaming, s.renameTo);

    if (s.deleteAsked)
    {
        s.deleteAsked = false;
        if (!s.selection.empty())
            ImGui::OpenPopup("Delete");
    }
    if (Confirm(
            "Delete",
            [&] {
                ImGui::Text("Delete %zu items from client_data?", s.selection.size());
                ImGui::TextDisabled("They go to .kom/trash: Ctrl+Z restores them while this window is open.");
                ImGui::TextDisabled("Files in the manifest show as deleted until a sync or commit drops them.");
            },
            "Delete"))
        DeleteItems(s, Selected(s));

    if (s.quitAsked)
    {
        s.quitAsked = false;
        ImGui::OpenPopup("Unsynced changes");
    }
    bool quit = false;
    if (ImGui::BeginPopupModal("Unsynced changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("%zu edits (archives) are not in the manifest yet.", s.data ? s.data->EditCount() : 0);
        if (ImGui::Button("Sync and close"))
        {
            SyncNow(s);
            quit = !s.messageIsError;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Close without syncing"))
        {
            quit = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    return quit;
}

void GlobalShortcuts(State& s)
{
    if (ImGui::GetIO().WantTextInput || s.Busy() || ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId))
        return;
    if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Z, ImGuiInputFlags_RouteGlobal))
        Undo(s);
    if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Y, ImGuiInputFlags_RouteGlobal) ||
        ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Z, ImGuiInputFlags_RouteGlobal))
        Redo(s);
    if (ImGui::Shortcut(ImGuiKey_F5, ImGuiInputFlags_RouteGlobal))
        Rescan(s);
}

} // namespace

int RunWindow(const std::filesystem::path& clientData)
{
    std::string error;
    auto sdl = app::Sdl::Init(error);
    if (!sdl)
    {
        std::fprintf(stderr, "x2_assets: %s\n", error.c_str());
        return 1;
    }
    app::WindowOptions windowOptions;
    windowOptions.title = "x2 assets";
    windowOptions.width = 1500;
    windowOptions.height = 900;
    auto window = app::Window::Create(windowOptions, error);
    if (!window)
    {
        std::fprintf(stderr, "x2_assets: %s\n", error.c_str());
        return 1;
    }
    auto device = render::Device::Create({}, error);
    if (!device)
    {
        std::fprintf(stderr, "x2_assets: %s\n", error.c_str());
        return 1;
    }
    render::WindowClaim claim(*device, window->Handle());
    if (!claim.Ok())
    {
        std::fprintf(stderr, "x2_assets: %s\n", device->LastError().c_str());
        return 1;
    }
    // A window drawn only after events: with FIFO (vsync) the first frame after a wait blocked 100-400 ms in
    // acquiring the swapchain image on Wayland (Hyprland, 2026-11-16). Mailbox does not wait.
    if (device->WindowSupportsPresentMode(window->Handle(), render::PresentMode::Mailbox))
        device->SetSwapchainParameters(window->Handle(), render::PresentMode::Mailbox);
    auto gui = app::ImGuiLayer::Create(*window, *device, {}, error);
    if (!gui)
    {
        std::fprintf(stderr, "x2_assets: %s\n", error.c_str());
        return 1;
    }

    State s;
    s.window = window->Handle();
    std::snprintf(s.packTo, sizeof s.packTo, "%s", ReadPref("pack_destination.txt").c_str());
    std::filesystem::path start = clientData;
    if (start.empty())
        start = PathFromUtf8(ReadPref("last_client_data.txt"));
    if (!start.empty())
        OpenFolder(s, std::filesystem::absolute(start));
    else
        s.Say("Open a client_data folder (made by x2_assets --cli import).");

    int eager = 2;  // frames drawn after an event before waiting again
    bool running = true;
    while (running)
    {
        if (eager > 0)
            --eager;
        else
            app::WaitForEvents(s.Busy() ? 50 : 500);
        running = app::PollEvents(
            [&](const SDL_Event& event) {
                eager = 3;
                gui->ProcessEvent(event);
                if (event.type == SDL_EVENT_DROP_FILE && event.drop.data)
                    s.dropped.emplace_back(event.drop.data);
            },
            window->Id());
        if (!running && (s.Busy() || (s.data && s.data->EditCount() > 0)))
        {
            running = true;
            s.quitAsked = !s.Busy();
        }
        {
            const std::lock_guard lock(s.pickedMutex);
            if (s.picked)
            {
                if (s.picking == Pick::PackDestination)
                {
                    std::snprintf(s.packTo, sizeof s.packTo, "%s", s.picked->c_str());
                    WritePref("pack_destination.txt", s.packTo);
                }
                else if (s.picking == Pick::Export)
                    ExportTo(s, PathFromUtf8(*s.picked));
                else if (!s.data || s.data->EditCount() == 0)
                    OpenFolder(s, PathFromUtf8(*s.picked));
                else
                    s.Say("Sync or revert the edits before opening another client_data", true);
                s.picked.reset();
            }
        }
        FinishJob(s);
        AddDropped(s);
        if (window->Minimized())
            continue;

        gui->NewFrame();
        GlobalShortcuts(s);
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const ImGuiWindowFlags fixed = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoTitleBar;
        const float line = ImGui::GetFrameHeightWithSpacing();
        const float bar = 2 * line + ImGui::GetStyle().WindowPadding.y * 2;
        const float status = line + ImGui::GetStyle().WindowPadding.y * 2;
        const float side = std::max(360.0f, viewport->WorkSize.x * 0.32f);
        const float body = viewport->WorkSize.y - bar - status;
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, bar));
        if (ImGui::Begin("Toolbar", nullptr, fixed))
            Toolbar(s, window->Handle());
        ImGui::End();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + bar));
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x - side, body));
        if (ImGui::Begin("Explorer", nullptr, fixed))
            Explorer(s);
        ImGui::End();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - side, viewport->WorkPos.y + bar));
        ImGui::SetNextWindowSize(ImVec2(side, body));
        if (ImGui::Begin("Inspector", nullptr, fixed))
            Inspector(s);
        ImGui::End();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + bar + body));
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, status));
        if (ImGui::Begin("Status", nullptr, fixed))
            StatusBar(s);
        ImGui::End();
        if (Popups(s))
            running = false;

        render::CommandBuffer* commands = device->AcquireCommandBuffer();
        render::SwapchainTexture swapchain;
        if (!commands || !commands->WaitAndAcquireSwapchainTexture(window->Handle(), swapchain))
        {
            if (commands)
                commands->Cancel();
            std::fprintf(stderr, "x2_assets: frame: %s\n", device->LastError().c_str());
            break;
        }
        gui->Render(*commands, swapchain.texture, {0.08f, 0.08f, 0.09f, 1.0f});
        commands->Submit();
    }
    if (s.job)
        s.job->thread.join();
    if (s.data)
        s.data->SaveHashCache();
    if (!s.trash.empty())
    {
        std::error_code ec;  // this run's deleted files go for good with its undo history
        std::filesystem::remove_all(s.trash, ec);
    }
    device->WaitIdle();
    return 0;
}

} // namespace x2::assets
