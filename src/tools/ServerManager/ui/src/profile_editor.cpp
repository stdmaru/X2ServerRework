#include "profile_editor.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <algorithm>
#include <filesystem>
#include <string_view>

namespace x2::servermanager {

namespace {

const ImVec4 kErrorColor = {0.95f, 0.40f, 0.40f, 1.0f};
const ImVec4 kOkColor = {0.40f, 0.85f, 0.45f, 1.0f};

std::filesystem::path Utf8Path(std::string_view text)
{
    return std::filesystem::path(std::u8string_view(reinterpret_cast<const char8_t*>(text.data()), text.size()));
}

std::string PathUtf8(const std::filesystem::path& path)
{
    const std::u8string text = path.lexically_normal().u8string();
    return std::string(text.begin(), text.end());
}

std::string Join(const std::vector<std::string>& values)
{
    std::string text;
    for (std::size_t i = 0; i < values.size(); ++i)
        text += (i ? "\n" : "") + values[i];
    return text;
}

std::vector<std::string> SplitLines(std::string_view text)
{
    std::vector<std::string> values;
    while (!text.empty()) {
        const std::size_t end = text.find('\n');
        std::string line(text.substr(0, end));
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (!line.empty())
            values.push_back(std::move(line));
        if (end == std::string_view::npos)
            break;
        text.remove_prefix(end + 1);
    }
    return values;
}

// One "label | field" row of a two-column table; the field takes the rest of the row.
void Label(const char* label, const char* help = nullptr)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(label);
    if (help)
        ImGui::SetItemTooltip("%s", help);
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-FLT_MIN);
}

bool BeginFields(const char* id)
{
    if (!ImGui::BeginTable(id, 2, ImGuiTableFlags_SizingStretchProp))
        return false;
    ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 9.0f);
    ImGui::TableSetupColumn("field", ImGuiTableColumnFlags_WidthStretch);
    return true;
}

bool TextField(const char* label, std::string& value, const char* help = nullptr, const char* hint = "")
{
    Label(label, help);
    ImGui::PushID(label);
    const bool changed = ImGui::InputTextWithHint("##v", hint, &value);
    ImGui::PopID();
    return changed;
}

// One value per line (arguments, ssh options).
bool LinesField(const char* label, std::vector<std::string>& values, const char* help)
{
    Label(label, help);
    std::string text = Join(values);
    ImGui::PushID(label);
    const float height = ImGui::GetTextLineHeight() * static_cast<float>(std::clamp<std::size_t>(values.size() + 1, 2, 6)) +
                         ImGui::GetStyle().FramePadding.y * 2.0f;
    const bool changed = ImGui::InputTextMultiline("##v", &text, ImVec2(-FLT_MIN, height));
    ImGui::PopID();
    if (changed)
        values = SplitLines(text);
    return changed;
}

// The add/remove/up/down buttons of a list; `selected` follows the moved item. True when the list changed.
template <class T, class Make>
bool ListButtons(std::vector<T>& items, int& selected, Make make)
{
    bool changed = false;
    if (ImGui::Button("Add")) {
        items.push_back(make());
        selected = static_cast<int>(items.size()) - 1;
        changed = true;
    }
    const bool valid = selected >= 0 && selected < static_cast<int>(items.size());
    ImGui::SameLine();
    ImGui::BeginDisabled(!valid);
    if (ImGui::Button("Remove")) {
        items.erase(items.begin() + selected);
        selected = std::min(selected, static_cast<int>(items.size()) - 1);
        changed = true;
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!valid || selected == 0);
    if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
        std::swap(items[static_cast<std::size_t>(selected)], items[static_cast<std::size_t>(selected - 1)]);
        --selected;
        changed = true;
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!valid || selected + 1 >= static_cast<int>(items.size()));
    if (ImGui::ArrowButton("##down", ImGuiDir_Down)) {
        std::swap(items[static_cast<std::size_t>(selected)], items[static_cast<std::size_t>(selected + 1)]);
        ++selected;
        changed = true;
    }
    ImGui::EndDisabled();
    return changed;
}

template <class T>
std::string UniqueName(const std::vector<T>& items, const std::string& base)
{
    for (int n = 1;; ++n) {
        const std::string name = n == 1 ? base : base + std::to_string(n);
        if (std::none_of(items.begin(), items.end(), [&](const T& item) { return item.name == name; }))
            return name;
    }
}

}  // namespace

void ProfileEditor::Open(const Profile& profile, const std::string& file)
{
    m_draft = profile;
    m_file = file;
    m_host = m_draft.hosts.empty() ? -1 : 0;
    m_server = m_draft.servers.empty() ? -1 : 0;
    m_message.clear();
    MarkSaved();
}

std::string ProfileEditor::BaseDir() const
{
    std::error_code ec;
    return m_file.empty() ? std::string() : PathUtf8(std::filesystem::absolute(Utf8Path(m_file), ec).parent_path());
}

bool ProfileEditor::Dirty() const
{
    return ProfileToJson(m_draft, BaseDir()) != m_saved;
}

void ProfileEditor::MarkSaved()
{
    m_saved = ProfileToJson(m_draft, BaseDir());
}

void ProfileEditor::RequestPick(PickKind kind, const std::string& start, Field field, int index)
{
    if (!m_picker)
        return;
    std::weak_ptr<Picks> picks = m_picks;
    m_picker(kind, start, [picks, field, index](std::string path) {
        if (path.empty())
            return;
        if (const auto queue = picks.lock()) {
            const std::lock_guard lock(queue->mutex);
            queue->items.push_back({field, index, std::move(path)});
        }
    });
}

bool ProfileEditor::Poll()
{
    std::vector<Pick> picks;
    {
        const std::lock_guard lock(m_picks->mutex);
        picks.swap(m_picks->items);
    }
    bool changed = false;
    for (Pick& pick : picks) {
        const auto host = static_cast<std::size_t>(pick.index);
        const auto server = static_cast<std::size_t>(pick.index);
        switch (pick.field) {
        case Field::ProfileX2ctl: m_draft.x2ctl = std::move(pick.path); break;
        case Field::ProfileControlDir: m_draft.controlDir = std::move(pick.path); break;
        case Field::HostIdentity:
            if (host >= m_draft.hosts.size())
                continue;
            m_draft.hosts[host].identityFile = std::move(pick.path);
            break;
        case Field::ServerDirectory:
            if (server >= m_draft.servers.size())
                continue;
            m_draft.servers[server].directory = std::move(pick.path);
            break;
        case Field::ServerProgram: {
            if (server >= m_draft.servers.size())
                continue;
            // a program in the server's directory is written ./name, as the default profile does
            ServerSpec& s = m_draft.servers[server];
            const std::filesystem::path program = Utf8Path(pick.path);
            std::error_code ec;
            if (!s.directory.empty() && std::filesystem::equivalent(program.parent_path(), Utf8Path(s.directory), ec))
                s.program = "./" + PathUtf8(program.filename());
            else
                s.program = std::move(pick.path);
            break;
        }
        }
        changed = true;
    }
    return changed;
}

bool ProfileEditor::PathField(const char* id, std::string& value, PickKind kind, Field field, int index, const char* hint)
{
    ImGui::PushID(id);
    const float button = ImGui::GetFrameHeight() * 1.6f;
    ImGui::SetNextItemWidth(-(button + ImGui::GetStyle().ItemSpacing.x));
    const bool changed = ImGui::InputTextWithHint("##v", hint, &value);
    ImGui::SameLine();
    ImGui::BeginDisabled(!m_picker);
    if (ImGui::Button("...", ImVec2(button, 0))) {
        std::string start = value;
        if (start.empty() || !Utf8Path(start).is_absolute())
            start = BaseDir();
        RequestPick(kind, start, field, index);
    }
    ImGui::EndDisabled();
    ImGui::SetItemTooltip(kind == PickKind::File ? "Choose a file" : "Choose a folder");
    ImGui::PopID();
    return changed;
}

ProfileEditor::Action ProfileEditor::Draw(bool canApply, const char* applyBlocked)
{
    Action action = Action::None;
    const std::string problem = m_draft.Validate();
    const bool dirty = Dirty();

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 28.0f);
    ImGui::InputTextWithHint("##file", "profile.json", &m_file);
    ImGui::SetItemTooltip("Where Save writes the profile");
    ImGui::SameLine();
    ImGui::BeginDisabled(!problem.empty() || m_file.empty());
    if (ImGui::Button("Save"))
        action = Action::Save;
    ImGui::SameLine();
    ImGui::BeginDisabled(!canApply);
    if (ImGui::Button("Save and apply"))
        action = Action::Apply;
    ImGui::EndDisabled();
    if (!canApply && applyBlocked)
        ImGui::SetItemTooltip("%s", applyBlocked);
    else
        ImGui::SetItemTooltip("Save, then load it as the window's profile");
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!dirty);
    if (ImGui::Button("Revert")) {
        std::string error;
        if (auto loaded = LoadProfile(m_file, error))
            Open(*loaded, m_file);
        else
            SetMessage(error, true);
    }
    ImGui::EndDisabled();
    ImGui::SetItemTooltip("Read the file again, dropping the changes");
    ImGui::SameLine();
    if (!problem.empty())
        ImGui::TextColored(kErrorColor, "%s", problem.c_str());
    else if (!m_message.empty() && (m_messageIsError || !dirty))
        ImGui::TextColored(m_messageIsError ? kErrorColor : kOkColor, "%s", m_message.c_str());
    else if (dirty)
        ImGui::TextDisabled("not saved");

    if (ImGui::BeginChild("profile", ImVec2(0, 0))) {
        if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
            DrawGeneral();
        if (ImGui::CollapsingHeader("Hosts", ImGuiTreeNodeFlags_DefaultOpen))
            DrawHosts();
        if (ImGui::CollapsingHeader("Servers", ImGuiTreeNodeFlags_DefaultOpen))
            DrawServers();
    }
    ImGui::EndChild();
    return action;
}

void ProfileEditor::DrawGeneral()
{
    if (!BeginFields("general"))
        return;
    Label("x2ctl", "This machine's x2ctl, for attaching to servers already running: a path, or a name in PATH");
    PathField("x2ctl", m_draft.x2ctl, PickKind::File, Field::ProfileX2ctl, 0, "x2ctl");
    Label("Control folder", "Where this machine's servers have their control sockets, when not the default "
                            "(servers in containers with a bind-mounted folder)");
    PathField("controlDir", m_draft.controlDir, PickKind::Folder, Field::ProfileControlDir, 0, "(default)");
    ImGui::EndTable();
}

void ProfileEditor::DrawHosts()
{
    ImGui::PushID("hosts");
    ListButtons(m_draft.hosts, m_host, [&] {
        HostSpec h;
        h.name = UniqueName(m_draft.hosts, "host");
        return h;
    });
    const float listWidth = ImGui::GetFontSize() * 12.0f;
    if (ImGui::BeginChild("list", ImVec2(listWidth, ImGui::GetFontSize() * 18.0f), ImGuiChildFlags_Borders)) {
        for (std::size_t i = 0; i < m_draft.hosts.size(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::Selectable(m_draft.hosts[i].name.empty() ? "(no name)" : m_draft.hosts[i].name.c_str(),
                                  m_host == static_cast<int>(i)))
                m_host = static_cast<int>(i);
            ImGui::PopID();
        }
        if (m_draft.hosts.empty())
            ImGui::TextDisabled("Only this machine");
    }
    ImGui::EndChild();
    ImGui::SameLine();
    if (ImGui::BeginChild("host", ImVec2(0, ImGui::GetFontSize() * 18.0f))) {
        if (m_host >= 0 && m_host < static_cast<int>(m_draft.hosts.size()))
            DrawHost(m_draft.hosts[static_cast<std::size_t>(m_host)]);
    }
    ImGui::EndChild();
    ImGui::PopID();
}

void ProfileEditor::DrawHost(HostSpec& host)
{
    if (!BeginFields("fields"))
        return;
    const std::string before = host.name;
    if (TextField("Name", host.name, "What servers refer to as their host") && !before.empty())
        for (ServerSpec& s : m_draft.servers)
            if (s.host == before)
                s.host = host.name;
    TextField("Destination", host.destination, "What ssh connects to: host, user@host, or a Host of ~/.ssh/config",
              "user@host");
    Label("Port", "ssh -p; 0: ssh's own (22, or the Port of ~/.ssh/config)");
    ImGui::InputInt("##port", &host.port, 0);
    host.port = std::clamp(host.port, 0, 65535);
    Label("Shell", "What runs the remote command there: a POSIX shell, or cmd.exe (Windows' OpenSSH)");
    int shell = host.shell == HostSpec::Shell::Cmd ? 1 : 0;
    if (ImGui::Combo("##shell", &shell, "sh (Linux)\0cmd (Windows)\0"))
        host.shell = shell == 1 ? HostSpec::Shell::Cmd : HostSpec::Shell::Sh;
    Label("ssh key", "A private key on this machine (ssh -i, IdentitiesOnly=yes); empty: ssh's own keys and agent");
    PathField("identity", host.identityFile, PickKind::File, Field::HostIdentity, m_host, "(ssh's default keys)");
    TextField("x2ctl", host.x2ctl, "x2ctl on that machine: a path, or a name in its PATH",
              host.shell == HostSpec::Shell::Cmd ? "C:\\x2\\dist\\x2ctl.exe" : "/opt/x2/dist/x2ctl");
    TextField("Control folder", host.controlDir, "Its control sockets there (x2ctl --dir); empty: x2ctl's default",
              "(default)");
    TextField("Start", host.start,
              "A command line run there to start one of its servers ({service}, {name}, {control}); empty: attach only",
              host.shell == HostSpec::Shell::Cmd ? "schtasks /run /tn \"x2 {service}\"" : "systemctl --user start x2@{service}");
    TextField("Stop", host.stop, "A command line run there to stop one of its servers; empty: the protocol's stop");
    LinesField("ssh command", host.ssh,
               "The ssh program and its options, one per line, before the destination; empty: ssh -T -o BatchMode=yes "
               "-o ConnectTimeout=10 (and a shared connection on Linux)");
    ImGui::EndTable();
}

void ProfileEditor::DrawServers()
{
    ImGui::PushID("servers");
    ListButtons(m_draft.servers, m_server, [&] {
        ServerSpec s;
        s.name = UniqueName(m_draft.servers, "Server");
        s.directory = BaseDir();
        return s;
    });
    ImGui::SameLine();
    ImGui::TextDisabled("(in start order among independent servers)");
    const float height = ImGui::GetFontSize() * 26.0f;
    if (ImGui::BeginChild("list", ImVec2(ImGui::GetFontSize() * 12.0f, height), ImGuiChildFlags_Borders)) {
        for (std::size_t i = 0; i < m_draft.servers.size(); ++i) {
            const ServerSpec& s = m_draft.servers[i];
            ImGui::PushID(static_cast<int>(i));
            const std::string label = (s.name.empty() ? std::string("(no name)") : s.name) +
                                      (s.host.empty() ? std::string() : " @" + s.host);
            if (ImGui::Selectable(label.c_str(), m_server == static_cast<int>(i)))
                m_server = static_cast<int>(i);
            ImGui::PopID();
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();
    if (ImGui::BeginChild("server", ImVec2(0, height))) {
        if (m_server >= 0 && m_server < static_cast<int>(m_draft.servers.size()))
            DrawServer(m_server, m_draft.servers[static_cast<std::size_t>(m_server)]);
    }
    ImGui::EndChild();
    ImGui::PopID();
}

void ProfileEditor::DrawServer(int index, ServerSpec& server)
{
    if (!BeginFields("fields"))
        return;
    const std::string before = server.name;
    if (TextField("Name", server.name) && !before.empty())
        for (ServerSpec& s : m_draft.servers)
            for (std::string& dep : s.after)
                if (dep == before)
                    dep = server.name;

    Label("Host", "Where it runs: here (ServerManager starts it), or a host (attached over ssh)");
    if (ImGui::BeginCombo("##host", server.host.empty() ? "(this machine)" : server.host.c_str())) {
        if (ImGui::Selectable("(this machine)", server.host.empty())) {
            server.host.clear();
            server.start.clear();
            server.stop.clear();
            server.service.clear();
        }
        for (const HostSpec& h : m_draft.hosts) {
            ImGui::PushID(&h);
            if (ImGui::Selectable(h.name.c_str(), server.host == h.name))
                server.host = h.name;
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
    const bool remote = !server.host.empty();
    if (remote) {
        TextField("Directory", server.directory, "Not used for a remote server");
        TextField("Program", server.program, "Its file name is the control socket name when Control is empty",
                  "./GameServer");
    } else {
        Label("Directory", "Working directory");
        PathField("directory", server.directory, PickKind::Folder, Field::ServerDirectory, index);
        Label("Program", "Relative to the directory (./LoginServer), or a path; .exe is added on Windows");
        PathField("program", server.program, PickKind::File, Field::ServerProgram, index, "./LoginServer");
        LinesField("Arguments", server.args, "One per line, before the console options ServerManager adds");
    }
    TextField("Control", server.control, "Control socket name; empty: the program's file name", server.ControlName().c_str());

    Label("After", "Servers that must be ready before this one starts");
    if (m_draft.servers.size() < 2) {
        ImGui::TextDisabled("(no other server)");
    } else if (ImGui::BeginTable("after", 3)) {
        for (const ServerSpec& other : m_draft.servers) {
            if (&other == &server || other.name.empty())
                continue;
            ImGui::TableNextColumn();
            const auto it = std::find(server.after.begin(), server.after.end(), other.name);
            bool checked = it != server.after.end();
            ImGui::PushID(&other);
            if (ImGui::Checkbox(other.name.c_str(), &checked)) {
                if (checked)
                    server.after.push_back(other.name);
                else
                    server.after.erase(it);
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    // dependencies that are no longer in the profile: shown so they can be removed
    for (std::size_t d = 0; d < server.after.size();) {
        if (m_draft.Find(server.after[d]) >= 0) {
            ++d;
            continue;
        }
        ImGui::PushID(static_cast<int>(d));
        ImGui::TextColored(kErrorColor, "%s (not in the profile)", server.after[d].c_str());
        ImGui::SameLine();
        const bool remove = ImGui::SmallButton("Remove");
        ImGui::PopID();
        if (remove)
            server.after.erase(server.after.begin() + static_cast<std::ptrdiff_t>(d));
        else
            ++d;
    }

    if (remote) {
        TextField("Service", server.service, "Its name in the host's service manager ({service}); empty: its name");
        TextField("Start", server.start, "Replaces the host's start command for this server", "(the host's)");
        TextField("Stop", server.stop, "Replaces the host's stop command for this server", "(the host's)");
    }
    ImGui::EndTable();
}

}  // namespace x2::servermanager
