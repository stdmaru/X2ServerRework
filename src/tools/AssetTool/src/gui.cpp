#include "gui.h"

#include "x2/app/imgui_layer.h"
#include "x2/assets/workspace.h"
#include "x2/kom/codec.h"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <unordered_map>

namespace x2::assets {

namespace {

constexpr std::size_t kStates = 5;

std::size_t Index(FileState s) { return static_cast<std::size_t>(s); }

ImVec4 StateColor(FileState state)
{
    switch (state)
    {
    case FileState::Modified: return {0.95f, 0.80f, 0.30f, 1.0f};
    case FileState::Deleted: return {0.95f, 0.40f, 0.40f, 1.0f};
    case FileState::Untracked: return {0.45f, 0.85f, 0.45f, 1.0f};
    case FileState::Added: return {0.50f, 0.75f, 1.00f, 1.0f};
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

// Where the window remembers the last client_data opened.
std::filesystem::path LastFolderFile()
{
    char* pref = SDL_GetPrefPath("x2", "x2_assets");
    if (!pref)
        return {};
    std::filesystem::path path = PathFromUtf8(pref);
    SDL_free(pref);
    return path / "last_client_data.txt";
}

struct Folder
{
    std::string name;  // last part
    std::string path;  // relative, '/'
    std::vector<std::size_t> children;  // folders
    std::vector<std::size_t> files;     // into ClientData::Files()
    std::array<std::size_t, kStates> counts{};  // under it, recursive
    std::size_t total = 0;
};

// Work on a copy of the data in the background (open, rescan, sync), swapped in when done.
struct Job
{
    std::thread thread;
    std::atomic<bool> done{false};
    std::atomic<std::size_t> progress{0}, total{0};
    std::string label;
    std::unique_ptr<ClientData> result;
    std::string error;
    std::string message;  // on success
    bool ok = false;
};

enum class View
{
    Folders,
    Archives,
};

enum class Selected
{
    None,
    File,
    Folder,
    Archive,
};

struct State
{
    std::filesystem::path root;
    std::unique_ptr<ClientData> data;
    std::unique_ptr<Job> job;
    unsigned generation = 0;  // bumped when `data` changes

    // Explorer
    std::vector<Folder> folders;  // [0]: the root
    std::vector<std::size_t> deleted;
    std::vector<std::vector<std::size_t>> byArchive;  // ArchiveOf, per manifest archive; then the unassigned
    unsigned archivesBuilt = ~0u;
    std::array<std::size_t, kStates> counts{};
    View view = View::Folders;
    char filter[128] = {};
    bool onlyChanges = false;
    std::vector<std::size_t> flat;  // filtered
    std::string flatKey;
    bool revealSelected = false;

    // Selection, by name so it survives a rescan
    Selected selected = Selected::None;
    std::string selection;  // file name, folder path or archive name

    // Inspector
    char moveTo[260] = {};
    int folderArchive = -1;

    std::string message;
    bool messageIsError = false;
    bool quitAsked = false;
    bool syncAsked = false;
    bool removeAsked = false;
    std::vector<std::string> dropped;

    std::string pickFrom;    // the folder dialog's start, alive while it is open
    std::mutex pickedMutex;  // the folder dialog answers from another thread
    std::optional<std::string> picked;

    void Say(std::string text, bool error = false)
    {
        message = std::move(text);
        messageIsError = error;
    }
    bool Busy() const { return job != nullptr; }
};

void BuildTree(State& s)
{
    s.folders.assign(1, Folder{});
    s.deleted.clear();
    s.counts = {};
    std::unordered_map<std::string, std::size_t> byPath{{"", 0}};
    const auto& files = s.data->Files();
    const auto folderOf = [&](const std::string& path) {
        // Creates the chain of folders down to `path`.
        std::size_t at = 0;
        std::size_t start = 0;
        while (start < path.size())
        {
            std::size_t end = path.find('/', start);
            if (end == std::string::npos)
                end = path.size();
            const std::string sub = path.substr(0, end);
            auto it = byPath.find(sub);
            if (it == byPath.end())
            {
                Folder f;
                f.name = path.substr(start, end - start);
                f.path = sub;
                s.folders.push_back(std::move(f));
                it = byPath.emplace(sub, s.folders.size() - 1).first;
                s.folders[at].children.push_back(it->second);
            }
            at = it->second;
            start = end + 1;
        }
        return at;
    };
    for (std::size_t i = 0; i < files.size(); ++i)
    {
        const ClientDataFile& f = files[i];
        ++s.counts[Index(f.state)];
        if (f.state == FileState::Deleted)
        {
            s.deleted.push_back(i);
            continue;
        }
        s.folders[folderOf(f.folder)].files.push_back(i);
        // Counts up the chain.
        std::string at = f.folder;
        for (;;)
        {
            Folder& folder = s.folders[byPath.at(at)];
            ++folder.counts[Index(f.state)];
            ++folder.total;
            if (at.empty())
                break;
            const auto slash = at.rfind('/');
            at = slash == std::string::npos ? std::string() : at.substr(0, slash);
        }
    }
    for (Folder& f : s.folders)
        std::sort(f.children.begin(), f.children.end(),
                  [&](std::size_t a, std::size_t b) { return Lower(s.folders[a].name) < Lower(s.folders[b].name); });
    s.flatKey.clear();
}

void BuildArchives(State& s)
{
    const auto& archives = s.data->GetManifest().archives;
    std::unordered_map<std::string, std::size_t> order;
    for (std::size_t a = 0; a < archives.size(); ++a)
        order.emplace(archives[a].name, a);
    s.byArchive.assign(archives.size() + 1, {});
    const auto& files = s.data->Files();
    for (std::size_t i = 0; i < files.size(); ++i)
    {
        const auto it = order.find(s.data->ArchiveOf(files[i]));
        s.byArchive[it == order.end() ? archives.size() : it->second].push_back(i);
    }
    s.archivesBuilt = s.generation;
}

void Changed(State& s)
{
    ++s.generation;
    s.flatKey.clear();
}

void StartJob(State& s, std::string label, std::unique_ptr<ClientData> data,
              std::function<bool(ClientData&, Job&, std::string&)> work)
{
    auto job = std::make_unique<Job>();
    job->label = std::move(label);
    job->result = std::move(data);
    Job* j = job.get();
    job->thread = std::thread([j, work = std::move(work)] {
        j->ok = work(*j->result, *j, j->error);
        j->done = true;
        app::WakeEventLoop();
    });
    s.job = std::move(job);
}

ClientData::Progress ProgressOf(Job& job)
{
    return [&job](std::size_t done, std::size_t total) {
        job.progress = done;
        job.total = total;
    };
}

void OpenFolder(State& s, const std::filesystem::path& root)
{
    if (s.Busy())
        return;
    s.root = root;
    StartJob(s, "Opening " + PathToUtf8(root), std::make_unique<ClientData>(),
             [root](ClientData& data, Job& job, std::string& error) {
                 return data.Open(root, 0, error, ProgressOf(job));
             });
}

void Rescan(State& s)
{
    if (s.Busy() || !s.data)
        return;
    StartJob(s, "Reading client_data", std::make_unique<ClientData>(*s.data),
             [](ClientData& data, Job& job, std::string& error) { return data.Rescan(0, error, ProgressOf(job)); });
}

void Sync(State& s)
{
    if (s.Busy() || !s.data)
        return;
    StartJob(s, "Syncing the manifest", std::make_unique<ClientData>(*s.data),
             [](ClientData& data, Job& job, std::string& error) {
                 SyncStats stats;
                 if (!data.Sync(0, stats, error))
                     return false;
                 char text[160];
                 std::snprintf(text, sizeof text, "Manifest synced: %zu files added, %zu dropped, %zu moved to another archive",
                               stats.added, stats.removed, stats.moved);
                 job.message = text;
                 return true;
             });
}

void FinishJob(State& s)
{
    if (!s.job || !s.job->done)
        return;
    s.job->thread.join();
    std::unique_ptr<Job> job = std::move(s.job);
    if (!job->ok)
    {
        s.Say(job->error, true);
        return;
    }
    s.data = std::move(job->result);
    if (!job->message.empty())
        s.Say(job->message);
    else if (s.message.empty() || s.messageIsError)
        s.Say("");
    BuildTree(s);
    Changed(s);
    std::ofstream(LastFolderFile(), std::ios::trunc) << PathToUtf8(s.root);
}

void Select(State& s, Selected what, std::string name)
{
    s.selected = what;
    s.selection = std::move(name);
    s.moveTo[0] = 0;
    s.folderArchive = -1;
}

// --- Explorer ---------------------------------------------------------------------------------------------------------

void CountsSuffix(const std::array<std::size_t, kStates>& counts)
{
    const auto part = [&](FileState state, const char* sign) {
        if (const std::size_t n = counts[Index(state)])
        {
            ImGui::SameLine();
            ImGui::TextColored(StateColor(state), "%s%zu", sign, n);
        }
    };
    part(FileState::Untracked, "+");
    part(FileState::Added, "*");
    part(FileState::Modified, "~");
    part(FileState::Deleted, "-");
}

void FileRow(State& s, std::size_t index)
{
    const ClientDataFile& f = s.data->Files()[index];
    const bool edited = s.data->HasArchiveEdit(f);
    ImGui::PushID(static_cast<int>(index));
    ImGui::PushStyleColor(ImGuiCol_Text, StateColor(f.state));
    const std::string label = edited ? f.name + "  (archive edited)" : f.name;
    if (ImGui::Selectable(label.c_str(), s.selected == Selected::File && Lower(s.selection) == Lower(f.name)))
        Select(s, Selected::File, f.name);
    ImGui::PopStyleColor();
    ImGui::PopID();
}

void FileList(State& s, const std::vector<std::size_t>& files)
{
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(files.size()));
    while (clipper.Step())
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            FileRow(s, files[static_cast<std::size_t>(i)]);
}

void FolderNode(State& s, std::size_t index)
{
    const Folder& folder = s.folders[index];
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                               ImGuiTreeNodeFlags_SpanAvailWidth;
    if (s.selected == Selected::Folder && s.selection == folder.path)
        flags |= ImGuiTreeNodeFlags_Selected;
    if (s.revealSelected && s.selected == Selected::File)
        if (const ClientDataFile* f = s.data->Find(s.selection);
            f && (f->folder == folder.path || f->folder.starts_with(folder.path + "/")))
            ImGui::SetNextItemOpen(true);
    const bool changed = folder.counts[Index(FileState::Modified)] || folder.counts[Index(FileState::Untracked)];
    if (changed)
        ImGui::PushStyleColor(ImGuiCol_Text, StateColor(folder.counts[Index(FileState::Modified)]
                                                            ? FileState::Modified
                                                            : FileState::Untracked));
    const bool open = ImGui::TreeNodeEx(folder.path.c_str(), flags, "%s", folder.name.c_str());
    if (changed)
        ImGui::PopStyleColor();
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        Select(s, Selected::Folder, folder.path);
    ImGui::SameLine();
    ImGui::TextDisabled("%zu", folder.total);
    CountsSuffix(folder.counts);
    if (!open)
        return;
    for (const std::size_t child : folder.children)
        FolderNode(s, child);
    FileList(s, folder.files);
    ImGui::TreePop();
}

void UpdateFlat(State& s)
{
    const std::string key = std::to_string(s.generation) + (s.onlyChanges ? "1" : "0") + s.filter;
    if (key == s.flatKey)
        return;
    s.flatKey = key;
    s.flat.clear();
    const std::string needle = Lower(s.filter);
    const auto& files = s.data->Files();
    for (std::size_t i = 0; i < files.size(); ++i)
    {
        if (s.onlyChanges && files[i].state == FileState::Unchanged && !s.data->HasArchiveEdit(files[i]))
            continue;
        if (!needle.empty() && Lower(files[i].name).find(needle) == std::string::npos)
            continue;
        s.flat.push_back(i);
    }
}

void FlatList(State& s)
{
    UpdateFlat(s);
    ImGui::TextDisabled("%zu files", s.flat.size());
    if (!ImGui::BeginTable("flat", 4,
                           ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable |
                               ImGuiTableFlags_BordersInnerV))
        return;
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 2.0f);
    ImGui::TableSetupColumn("Folder", ImGuiTableColumnFlags_WidthStretch, 2.0f);
    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch, 1.0f);
    ImGui::TableSetupColumn("Archive", ImGuiTableColumnFlags_WidthStretch, 1.0f);
    ImGui::TableHeadersRow();
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(s.flat.size()));
    while (clipper.Step())
        for (int r = clipper.DisplayStart; r < clipper.DisplayEnd; ++r)
        {
            const std::size_t i = s.flat[static_cast<std::size_t>(r)];
            const ClientDataFile& f = s.data->Files()[i];
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::PushID(static_cast<int>(i));
            ImGui::PushStyleColor(ImGuiCol_Text, StateColor(f.state));
            if (ImGui::Selectable(f.name.c_str(), s.selected == Selected::File && Lower(s.selection) == Lower(f.name),
                                  ImGuiSelectableFlags_SpanAllColumns))
                Select(s, Selected::File, f.name);
            ImGui::PopStyleColor();
            ImGui::PopID();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(f.state == FileState::Deleted ? "-" : f.folder.empty() ? "(root)" : f.folder.c_str());
            ImGui::TableNextColumn();
            ImGui::TextColored(StateColor(f.state), "%s", std::string(ToString(f.state)).c_str());
            ImGui::TableNextColumn();
            const std::string archive = s.data->ArchiveOf(f);
            ImGui::Text("%s%s", archive.empty() ? "(none)" : archive.c_str(), s.data->HasArchiveEdit(f) ? " *" : "");
        }
    ImGui::EndTable();
}

void ArchiveTree(State& s)
{
    if (s.archivesBuilt != s.generation)
        BuildArchives(s);
    const auto& archives = s.data->GetManifest().archives;
    for (std::size_t a = 0; a < s.byArchive.size(); ++a)
    {
        const auto& files = s.byArchive[a];
        if (a == archives.size() && files.empty())
            continue;
        const std::string name = a < archives.size() ? archives[a].name : "(no archive)";
        std::array<std::size_t, kStates> counts{};
        for (const std::size_t i : files)
            ++counts[Index(s.data->Files()[i].state)];
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (s.selected == Selected::Archive && s.selection == name)
            flags |= ImGuiTreeNodeFlags_Selected;
        const bool open = ImGui::TreeNodeEx(name.c_str(), flags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            Select(s, Selected::Archive, name);
        ImGui::SameLine();
        ImGui::TextDisabled("%zu", files.size());
        CountsSuffix(counts);
        if (open)
        {
            FileList(s, files);
            ImGui::TreePop();
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
    if (s.onlyChanges || s.filter[0])
    {
        FlatList(s);
        return;
    }
    ImGui::BeginChild("tree");
    if (s.view == View::Folders)
    {
        if (!s.deleted.empty())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, StateColor(FileState::Deleted));
            const bool open = ImGui::TreeNodeEx("##deleted", ImGuiTreeNodeFlags_SpanAvailWidth, "Deleted (%zu)",
                                                s.deleted.size());
            ImGui::PopStyleColor();
            if (open)
            {
                FileList(s, s.deleted);
                ImGui::TreePop();
            }
        }
        const Folder& root = s.folders[0];
        for (const std::size_t child : root.children)
            FolderNode(s, child);
        FileList(s, root.files);
    }
    else
    {
        ArchiveTree(s);
    }
    s.revealSelected = false;
    ImGui::EndChild();
}

// --- Inspector --------------------------------------------------------------------------------------------------------

bool ArchiveCombo(const char* label, const Manifest& manifest, const std::string& current, int& chosen)
{
    bool picked = false;
    if (ImGui::BeginCombo(label, current.empty() ? "(none)" : current.c_str(), ImGuiComboFlags_HeightLarge))
    {
        for (std::size_t a = 0; a < manifest.archives.size(); ++a)
        {
            const std::string& name = manifest.archives[a].name;
            if (ImGui::Selectable(name.c_str(), name == current))
            {
                chosen = static_cast<int>(a);
                picked = true;
            }
        }
        ImGui::EndCombo();
    }
    return picked;
}

void Row(const char* label, const std::string& value)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TextDisabled("%s", label);
    ImGui::TableNextColumn();
    ImGui::TextWrapped("%s", value.c_str());
}

void FileInspector(State& s, const ClientDataFile& f)
{
    ImGui::TextUnformatted(f.name.c_str());
    ImGui::TextColored(StateColor(f.state), "%s", StateLabel(f.state));
    ImGui::Separator();
    if (ImGui::BeginTable("file", 2, ImGuiTableFlags_SizingStretchProp))
    {
        Row("Folder", f.state == FileState::Deleted ? "(not in client_data)" : f.folder.empty() ? "(root)" : f.folder);
        if (f.state != FileState::Deleted)
        {
            Row("Size", FormatSize(f.size));
            Row("SHA-256", f.sha256);
        }
        if (f.record >= 0)
        {
            const FileRecord& r = s.data->GetManifest().files[static_cast<std::size_t>(f.record)];
            Row("Record", r.name);
            Row("Archive", r.archive + ", entry " + std::to_string(r.index));
            Row("Algorithm", std::to_string(r.algorithm));
            Row("Encoding", std::string(ToString(r.encoding)));
            Row("Form", std::string(ToString(r.form)));
            if (r.encoding != Encoding::New)
                Row("Original", FormatSize(r.size) + " (" + FormatSize(r.storedSize) + " stored)");
        }
        ImGui::EndTable();
    }
    if (f.state == FileState::Deleted)
        return;

    ImGui::SeparatorText("Target archive");
    const Manifest& manifest = s.data->GetManifest();
    const std::string archive = s.data->ArchiveOf(f);
    int chosen = -1;
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ArchiveCombo("##archive", manifest, archive, chosen))
    {
        std::string error;
        if (!s.data->SetArchive(f.name, manifest.archives[static_cast<std::size_t>(chosen)].name, error))
            s.Say(error, true);
        Changed(s);
    }
    if (s.data->HasArchiveEdit(f))
    {
        ImGui::TextColored(StateColor(FileState::Modified), "Edited: applied by Sync data structure");
        if (ImGui::Button("Revert"))
        {
            std::string error;
            const std::string base = f.record >= 0
                                         ? manifest.files[static_cast<std::size_t>(f.record)].archive
                                         : s.data->DefaultArchive(f.folder);
            if (!base.empty() && !s.data->SetArchive(f.name, base, error))
                s.Say(error, true);
            Changed(s);
        }
    }
    else if (f.record < 0)
    {
        ImGui::TextDisabled("The folder's default (its most common archive)");
    }

    ImGui::SeparatorText("File");
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::InputTextWithHint("##move", "Folder to move it to (e.g. ui/textures)", s.moveTo, sizeof s.moveTo);
    ImGui::BeginDisabled(s.Busy() || !s.moveTo[0]);
    if (ImGui::Button("Move"))
    {
        std::string error;
        if (s.data->MoveFile(f.name, s.moveTo, error))
        {
            s.Say("Moved " + f.name + " to " + s.moveTo);
            s.revealSelected = true;
            Rescan(s);
        }
        else
            s.Say(error, true);
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(s.Busy());
    if (ImGui::Button("Delete from client_data..."))
        s.removeAsked = true;
    ImGui::EndDisabled();
}

void FolderInspector(State& s, const std::string& path)
{
    const auto it = std::find_if(s.folders.begin(), s.folders.end(), [&](const Folder& f) { return f.path == path; });
    if (it == s.folders.end())
        return;
    ImGui::TextUnformatted(path.empty() ? "(root)" : path.c_str());
    ImGui::TextDisabled("Folder, %zu files", it->total);
    ImGui::Separator();
    if (ImGui::BeginTable("folder", 2, ImGuiTableFlags_SizingStretchProp))
    {
        for (const FileState st : {FileState::Unchanged, FileState::Modified, FileState::Untracked, FileState::Added})
            if (it->counts[Index(st)])
                Row(std::string(ToString(st)).c_str(), std::to_string(it->counts[Index(st)]));
        const std::string def = s.data->DefaultArchive(path);
        Row("New files go to", def.empty() ? "(no archive)" : def);
        ImGui::EndTable();
    }
    ImGui::SeparatorText("Target archive of every file in it");
    const Manifest& manifest = s.data->GetManifest();
    const std::string current =
        s.folderArchive >= 0 ? manifest.archives[static_cast<std::size_t>(s.folderArchive)].name : std::string();
    ImGui::SetNextItemWidth(-FLT_MIN);
    ArchiveCombo("##folderArchive", manifest, current, s.folderArchive);
    ImGui::BeginDisabled(s.Busy() || s.folderArchive < 0);
    if (ImGui::Button("Apply to the folder and its subfolders"))
    {
        std::size_t count = 0;
        std::string error;
        if (s.data->SetFolderArchive(path, current, count, error))
            s.Say(std::to_string(count) + " files will go to " + current + " (applied by Sync data structure)");
        else
            s.Say(error, true);
        Changed(s);
    }
    ImGui::EndDisabled();
    ImGui::TextDisabled("Drop files on the window to add them to this folder.");
}

void ArchiveInspector(State& s, const std::string& name)
{
    ImGui::TextUnformatted(name.c_str());
    if (s.archivesBuilt != s.generation)
        BuildArchives(s);
    const auto& archives = s.data->GetManifest().archives;
    for (std::size_t a = 0; a < archives.size(); ++a)
        if (archives[a].name == name)
        {
            ImGui::TextDisabled("Archive %zu of %zu in load order, %zu files after a sync", a + 1, archives.size(),
                                s.byArchive[a].size());
            ImGui::TextDisabled("The manifest lists %u entries now.", archives[a].count);
        }
}

void Inspector(State& s)
{
    if (!s.data)
        return;
    switch (s.selected)
    {
    case Selected::File:
        if (const ClientDataFile* f = s.data->Find(s.selection))
        {
            ImGui::BeginDisabled(s.Busy());  // edits made now would be lost when the job's copy comes back
            FileInspector(s, *f);
            ImGui::EndDisabled();
        }
        break;
    case Selected::Folder:
        ImGui::BeginDisabled(s.Busy());
        FolderInspector(s, s.selection);
        ImGui::EndDisabled();
        break;
    case Selected::Archive: ArchiveInspector(s, s.selection); break;
    case Selected::None: ImGui::TextDisabled("Select a file, a folder or an archive."); break;
    }
}

// --- Toolbar and popups -----------------------------------------------------------------------------------------------

void PickFolder(State& s, SDL_Window* window)
{
    s.pickFrom = PathToUtf8(s.root);
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

void Toolbar(State& s, SDL_Window* window)
{
    ImGui::BeginDisabled(s.Busy());
    if (ImGui::Button("Open..."))
        PickFolder(s, window);
    ImGui::SameLine();
    if (ImGui::Button("Rescan (F5)"))
        Rescan(s);
    ImGui::SameLine();
    const std::size_t edits = s.data ? s.data->EditCount() : 0;
    const std::size_t pending = s.counts[Index(FileState::Untracked)] + s.counts[Index(FileState::Deleted)] + edits;
    ImGui::BeginDisabled(!s.data || pending == 0);
    if (ImGui::Button("Sync data structure"))
        s.syncAsked = true;
    ImGui::EndDisabled();
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Writes the manifest: adds the %zu new files, drops the %zu deleted ones and applies %zu "
                          "archive edits.\nModified files stay modified: the manifest keeps their original.",
                          s.counts[Index(FileState::Untracked)], s.counts[Index(FileState::Deleted)], edits);
    ImGui::SameLine();
    ImGui::TextDisabled("%s", s.root.empty() ? "No client_data open" : PathToUtf8(s.root).c_str());
    if (s.data)
    {
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();
        ImGui::TextColored(StateColor(FileState::Untracked), "%zu new", s.counts[Index(FileState::Untracked)]);
        ImGui::SameLine();
        ImGui::TextColored(StateColor(FileState::Modified), "%zu modified", s.counts[Index(FileState::Modified)]);
        ImGui::SameLine();
        ImGui::TextColored(StateColor(FileState::Deleted), "%zu deleted", s.counts[Index(FileState::Deleted)]);
        ImGui::SameLine();
        ImGui::TextColored(StateColor(FileState::Added), "%zu added", s.counts[Index(FileState::Added)]);
        if (edits)
        {
            ImGui::SameLine();
            ImGui::TextColored(StateColor(FileState::Modified), "| %zu unsynced archive edits", edits);
        }
    }
}

void StatusBar(State& s)
{
    if (s.job)
    {
        const std::size_t total = s.job->total, done = s.job->progress;
        char overlay[128];
        std::snprintf(overlay, sizeof overlay, "%s: %zu / %zu files", s.job->label.c_str(), done, total);
        ImGui::ProgressBar(total ? static_cast<float>(done) / static_cast<float>(total) : 0.0f, ImVec2(-FLT_MIN, 0),
                           overlay);
    }
    else if (!s.message.empty())
    {
        if (s.messageIsError)
            ImGui::TextColored(StateColor(FileState::Deleted), "%s", s.message.c_str());
        else
            ImGui::TextUnformatted(s.message.c_str());
    }
}

// True when the window should close.
bool Popups(State& s)
{
    if (s.syncAsked)
    {
        s.syncAsked = false;
        if (s.counts[Index(FileState::Deleted)] == 0)
            Sync(s);
        else
            ImGui::OpenPopup("Sync data structure");
    }
    if (ImGui::BeginPopupModal("Sync data structure", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("%zu deleted files will be dropped from the manifest: their original entries leave the archives.",
                    s.counts[Index(FileState::Deleted)]);
        ImGui::TextDisabled("The previous manifest is kept as .kom/manifest.tsv.bak.");
        if (ImGui::Button("Sync"))
        {
            Sync(s);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    if (s.removeAsked)
    {
        s.removeAsked = false;
        ImGui::OpenPopup("Delete file");
    }
    if (ImGui::BeginPopupModal("Delete file", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Delete %s from client_data? This cannot be undone.", s.selection.c_str());
        if (const ClientDataFile* f = s.data ? s.data->Find(s.selection) : nullptr; f && f->record >= 0)
            ImGui::TextDisabled("It is in the manifest: it shows as deleted until a sync drops it.");
        if (ImGui::Button("Delete"))
        {
            std::string error;
            if (s.data && s.data->RemoveFile(s.selection, error))
            {
                s.Say("Deleted " + s.selection);
                Rescan(s);
            }
            else
                s.Say(error, true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    if (s.quitAsked)
    {
        s.quitAsked = false;
        ImGui::OpenPopup("Unsynced changes");
    }
    bool quit = false;
    if (ImGui::BeginPopupModal("Unsynced changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("%zu archive edits are not in the manifest yet.", s.data ? s.data->EditCount() : 0);
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

void AddDropped(State& s)
{
    if (s.dropped.empty() || !s.data || s.Busy())
        return;
    std::size_t added = 0;
    std::string error;
    for (const std::string& path : s.dropped)
    {
        const std::filesystem::path source = PathFromUtf8(path);
        std::string folder;
        if (s.selected == Selected::Folder)
            folder = s.selection;
        else if (const ClientDataFile* f = s.selected == Selected::File ? s.data->Find(s.selection) : nullptr;
                 f && f->state != FileState::Deleted)
            folder = f->folder;
        else
            folder = s.data->FolderForNew(PathToUtf8(source.filename()));
        if (s.data->AddFile(source, folder, error))
            ++added;
        else
            s.Say(error, true);
    }
    s.dropped.clear();
    if (added)
    {
        if (!s.messageIsError)
            s.Say(std::to_string(added) + " files added (a sync records them)");
        Rescan(s);
    }
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
    auto gui = app::ImGuiLayer::Create(*window, *device, {}, error);
    if (!gui)
    {
        std::fprintf(stderr, "x2_assets: %s\n", error.c_str());
        return 1;
    }

    State s;
    std::filesystem::path start = clientData;
    if (start.empty())
    {
        std::ifstream in(LastFolderFile());
        std::string last;
        if (std::getline(in, last) && !last.empty())
            start = PathFromUtf8(last);
    }
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
                if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F5)
                    Rescan(s);
                if (event.type == SDL_EVENT_WINDOW_FOCUS_GAINED && s.data)
                    Rescan(s);  // files may have changed outside
            },
            window->Id());
        if (!running && s.data && s.data->EditCount() > 0)
        {
            running = true;
            s.quitAsked = true;
        }
        {
            const std::lock_guard lock(s.pickedMutex);
            if (s.picked)
            {
                if (!s.data || s.data->EditCount() == 0)
                    OpenFolder(s, PathFromUtf8(*s.picked));
                else
                    s.Say("Sync or revert the archive edits before opening another client_data", true);
                s.picked.reset();
            }
        }
        FinishJob(s);
        AddDropped(s);
        if (window->Minimized())
            continue;

        gui->NewFrame();
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const ImGuiWindowFlags fixed = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoTitleBar;
        const float bar = ImGui::GetFrameHeightWithSpacing() + ImGui::GetStyle().WindowPadding.y * 2;
        const float side = std::max(360.0f, viewport->WorkSize.x * 0.32f);
        const float body = viewport->WorkSize.y - 2 * bar;
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
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, bar));
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
    device->WaitIdle();
    return 0;
}

} // namespace x2::assets
