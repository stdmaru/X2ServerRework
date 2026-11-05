#include "manager_ui.h"

#include "x2/platform/control.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <string_view>

namespace x2::servermanager {

using namespace std::chrono_literals;

namespace {

constexpr auto kDiscoverEvery = 10s;

ImVec4 PhaseColor(Phase phase)
{
    switch (phase) {
    case Phase::Ready: return {0.40f, 0.85f, 0.45f, 1.0f};
    case Phase::Waiting:
    case Phase::Starting: return {0.95f, 0.80f, 0.30f, 1.0f};
    case Phase::Stopping: return {0.95f, 0.60f, 0.25f, 1.0f};
    case Phase::Failed: return {0.95f, 0.35f, 0.35f, 1.0f};
    case Phase::Offline: break;
    }
    return {0.55f, 0.55f, 0.58f, 1.0f};
}

ImVec4 LevelColor(LogLevel level)
{
    switch (level) {
    case LogLevel::Error: return {0.95f, 0.40f, 0.40f, 1.0f};
    case LogLevel::Warn: return {0.95f, 0.80f, 0.35f, 1.0f};
    case LogLevel::Note: return {0.60f, 0.70f, 0.85f, 1.0f};
    case LogLevel::Info: break;
    }
    return ImGui::GetStyleColorVec4(ImGuiCol_Text);
}

const char* LevelTag(LogLevel level)
{
    switch (level) {
    case LogLevel::Error: return "ERROR";
    case LogLevel::Warn: return "WARN";
    case LogLevel::Note: return "NOTE";
    case LogLevel::Info: break;
    }
    return "INFO";
}

// Everything an entry shows, possibly several lines: message, BUILD_LOG fields, extra text lines.
std::string EntryText(const LogEntry& entry)
{
    std::string text;
    if (entry.kind == EntryKind::Command)
        text = "> ";
    else if (entry.kind == EntryKind::Event)
        text = "-- ";
    text += entry.log.message;
    for (const LogField& field : entry.log.fields)
        text += "\n    " + field.name + " = " + field.value;
    for (const std::string& line : entry.log.text)
        text += "\n    " + line;
    while (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
        text.pop_back();
    return text;
}

std::uint32_t LineCount(std::string_view text)
{
    return 1 + static_cast<std::uint32_t>(std::count(text.begin(), text.end(), '\n'));
}

std::string_view Line(std::string_view text, std::uint32_t index)
{
    for (; index > 0; --index) {
        const std::size_t end = text.find('\n');
        if (end == std::string_view::npos)
            return {};
        text.remove_prefix(end + 1);
    }
    return text.substr(0, text.find('\n'));
}

// Lua history: Up/Down in the prompt.
int HistoryCallback(ImGuiInputTextCallbackData* data)
{
    auto& view = *static_cast<std::pair<std::vector<std::string>*, int*>*>(data->UserData);
    std::vector<std::string>& history = *view.first;
    int& pos = *view.second;
    if (data->EventFlag != ImGuiInputTextFlags_CallbackHistory || history.empty())
        return 0;
    const int previous = pos;
    if (data->EventKey == ImGuiKey_UpArrow)
        pos = pos < 0 ? static_cast<int>(history.size()) - 1 : std::max(0, pos - 1);
    else if (data->EventKey == ImGuiKey_DownArrow && pos >= 0)
        pos = pos + 1 < static_cast<int>(history.size()) ? pos + 1 : -1;
    if (pos != previous) {
        data->DeleteChars(0, data->BufTextLen);
        if (pos >= 0)
            data->InsertChars(0, history[static_cast<std::size_t>(pos)].c_str());
    }
    return 0;
}

}  // namespace

ManagerUi::ManagerUi(std::function<void()> wake) : m_wake(std::move(wake)), m_pathInput("dist") {}

ManagerUi::~ManagerUi() = default;

std::string ManagerUi::FindProfile(const std::vector<std::string>& args)
{
    if (args.size() > 1)
        return args[1];
    std::error_code ec;
    if (std::filesystem::is_regular_file("servermanager.json", ec))
        return "servermanager.json";
    if (std::filesystem::is_directory("dist", ec))
        return "dist";
    return {};
}

bool ManagerUi::Load(const std::string& path, std::string& error)
{
    Profile profile;
    std::error_code ec;
    if (std::filesystem::is_directory(path, ec)) {
        profile = DefaultProfile(std::filesystem::absolute(path, ec).lexically_normal().string());
    } else {
        auto loaded = LoadProfile(path, error);
        if (!loaded)
            return false;
        profile = std::move(*loaded);
    }
    m_orchestrator.reset();
    m_orchestrator = std::make_unique<Orchestrator>(std::move(profile));
    if (m_wake)
        m_orchestrator->Events().SetWake(m_wake);
    m_views = std::vector<ServerView>(m_orchestrator->Size());
    m_profilePath = path;
    m_pathInput = path;
    m_selected = 0;
    m_nextDiscover = {};
    return true;
}

bool ManagerUi::Update(Clock::time_point now)
{
    if (!m_orchestrator)
        return false;
    bool changed = false;
    if (now >= m_nextDiscover) {
        m_orchestrator->Discover();
        m_nextDiscover = now + kDiscoverEvery;
        changed = true;
    }
    changed = m_orchestrator->Update(now) || changed;

    if (m_quitting) {
        bool started = false;
        for (std::size_t i = 0; i < m_orchestrator->Size(); ++i) {
            const auto& s = m_orchestrator->At(i);
            started = started || (s.owned && s.phase != Phase::Offline && s.phase != Phase::Failed);
        }
        if (!started)
            m_quit = true;
    }
    return changed;
}

int ManagerUi::IdleTimeoutMs() const
{
    return m_orchestrator && m_orchestrator->Busy() ? 100 : 500;
}

void ManagerUi::RequestQuit()
{
    bool started = false;
    if (m_orchestrator)
        for (std::size_t i = 0; i < m_orchestrator->Size(); ++i) {
            const auto& s = m_orchestrator->At(i);
            started = started || (s.owned && s.phase != Phase::Offline && s.phase != Phase::Failed);
        }
    if (!started)
        m_quit = true;      // attached servers keep running: the orchestrator detaches them
    else
        m_quitAsked = true;
}

void ManagerUi::UpdateRows(const LogStore& logs, ServerView& view)
{
    // new filter or cleared store: start over; otherwise drop what the store dropped and add the new entries
    if (!(view.rowsFilter == view.filter) || view.rowsGeneration != logs.Generation()) {
        view.rows.clear();
        view.rowsLastSeq = 0;
        view.rowsFilter = view.filter;
        view.rowsGeneration = logs.Generation();
    }
    while (!view.rows.empty() && view.rows.front().first < logs.FirstSeq())
        view.rows.pop_front();
    const std::vector<std::uint64_t>& seqs = view.view.Seqs();
    for (auto it = std::upper_bound(seqs.begin(), seqs.end(), view.rowsLastSeq); it != seqs.end(); ++it) {
        if (const LogEntry* entry = logs.Find(*it)) {
            const std::uint32_t lines = LineCount(EntryText(*entry));
            for (std::uint32_t line = 0; line < lines; ++line)
                view.rows.emplace_back(*it, line);
        }
        view.rowsLastSeq = *it;
    }
}

void ManagerUi::Draw()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::Begin("ServerManager", nullptr, flags);

    DrawToolbar();
    if (m_orchestrator && m_orchestrator->Size() > 0) {
        const float listWidth = ImGui::GetFontSize() * 26.0f;
        ImGui::BeginChild("servers", ImVec2(listWidth, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
        DrawServers();
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("selected", ImVec2(0, 0));
        DrawSelected();
        ImGui::EndChild();
    } else if (m_orchestrator) {
        ImGui::TextDisabled("The profile has no servers.");
    }
    DrawQuitDialog();
    ImGui::End();
}

void ManagerUi::DrawToolbar()
{
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 24.0f);
    const bool enter = ImGui::InputTextWithHint("##profile", "profile.json or dist folder", &m_pathInput,
                                                ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    if (ImGui::Button("Load") || enter) {
        std::string error;
        m_loadError = Load(m_pathInput, error) ? std::string() : error;
    }
    if (!m_loadError.empty()) {
        ImGui::SameLine();
        ImGui::TextColored(LevelColor(LogLevel::Error), "%s", m_loadError.c_str());
    }
    if (!m_orchestrator) {
        ImGui::TextDisabled("Load a profile (JSON) or the dist folder of the server package.");
        return;
    }

    ImGui::SameLine();
    ImGui::BeginDisabled(m_quitting);
    if (ImGui::Button("Start all"))
        m_orchestrator->StartAll();
    ImGui::SameLine();
    if (ImGui::Button("Stop all"))
        m_orchestrator->StopAll();
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button("Refresh"))
        m_nextDiscover = {};
    ImGui::SetItemTooltip("Look for running servers again (here, and x2ctl list on each host)");

    for (std::size_t h = 0; h < m_orchestrator->Hosts(); ++h) {
        const auto& host = m_orchestrator->HostAt(h);
        ImGui::SameLine();
        if (host.running)
            ImGui::TextColored(PhaseColor(Phase::Ready), "%s", host.spec.name.c_str());
        else if (host.listing)
            ImGui::TextDisabled("%s...", host.spec.name.c_str());
        else
            ImGui::TextColored(PhaseColor(Phase::Failed), "%s", host.spec.name.c_str());
        if (ImGui::BeginItemTooltip()) {
            ImGui::Text("%s", host.spec.destination.c_str());
            if (!host.error.empty())
                ImGui::TextUnformatted(host.error.c_str());
            else if (host.running)
                ImGui::Text("%zu servers running", host.running->size());
            ImGui::EndTooltip();
        }
    }
}

void ManagerUi::DrawServers()
{
    Orchestrator& o = *m_orchestrator;
    const ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable("list", 3, flags)) {
        ImGui::TableSetupColumn("Server", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Phase");
        ImGui::TableSetupColumn("Log");
        ImGui::TableHeadersRow();
        for (std::size_t i = 0; i < o.Size(); ++i) {
            const auto& s = o.At(i);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            const std::string label = s.spec.name + (s.Remote() ? " @" + s.spec.host : std::string()) + "##" + std::to_string(i);
            if (ImGui::Selectable(label.c_str(), m_selected == static_cast<int>(i), ImGuiSelectableFlags_SpanAllColumns))
                m_selected = static_cast<int>(i);
            ImGui::TableNextColumn();
            const auto listed = o.Listed(static_cast<int>(i));
            if (s.phase == Phase::Offline && listed == true)
                ImGui::TextColored(PhaseColor(Phase::Starting), "running");    // not connected, but its socket is there
            else
                ImGui::TextColored(PhaseColor(s.phase), "%s", ToString(s.phase));
            ImGui::TableNextColumn();
            const auto errors = s.session.Logs().Count(LogLevel::Error);
            const auto warnings = s.session.Logs().Count(LogLevel::Warn);
            if (errors)
                ImGui::TextColored(LevelColor(LogLevel::Error), "%llu", static_cast<unsigned long long>(errors));
            else if (warnings)
                ImGui::TextColored(LevelColor(LogLevel::Warn), "%llu", static_cast<unsigned long long>(warnings));
            else
                ImGui::TextDisabled("-");
        }
        ImGui::EndTable();
    }

    ImGui::Separator();
    if (m_selected < 0 || static_cast<std::size_t>(m_selected) >= o.Size())
        return;
    const int i = m_selected;
    const auto& s = o.At(static_cast<std::size_t>(i));
    const bool connected = s.phase == Phase::Starting || s.phase == Phase::Ready || s.phase == Phase::Stopping;
    const bool idle = s.phase == Phase::Offline || s.phase == Phase::Failed;

    ImGui::BeginDisabled(!idle || m_quitting);
    const bool attachOnly = s.Remote() && !o.HasStartCommand(i);
    if (ImGui::Button(attachOnly ? "Attach when ready" : "Start"))
        o.Start(i);
    ImGui::SetItemTooltip(attachOnly ? "A remote server runs as its host's service: once its dependencies are ready, attach"
                          : s.Remote() ? "Once its dependencies are ready, run its host's start command, then attach"
                                       : "Start it (and what it depends on), or attach if it is already running");
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(s.phase == Phase::Stopping || (idle && o.Listed(i) != true && !o.HasStopCommand(i)));
    if (ImGui::Button("Stop"))
        o.Stop(i);
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!idle);
    if (ImGui::Button("Attach"))
        o.Attach(i);
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!connected || s.owned);
    if (ImGui::Button("Detach"))
        o.Detach(i);
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!connected || !s.owned);
    if (ImGui::Button("Kill"))
        o.Kill(i);
    ImGui::EndDisabled();

    ImGui::Text("Server state: %s", ToString(s.session.State()));
    if (s.session.GotHello())
        ImGui::Text("PID %lld, %s", static_cast<long long>(s.session.Pid()),
                    s.owned ? "started here" : s.Remote() ? ("attached on " + s.spec.host).c_str() : "attached");
    if (s.exitCode)
        ImGui::Text("Last exit code: %d", *s.exitCode);
    if (!s.error.empty())
        ImGui::TextColored(LevelColor(LogLevel::Error), "%s", s.error.c_str());
    if (!s.spec.after.empty()) {
        std::string after;
        for (const std::string& dep : s.spec.after)
            after += (after.empty() ? "" : ", ") + dep;
        ImGui::TextDisabled("After: %s", after.c_str());
    }
}

void ManagerUi::DrawSelected()
{
    Orchestrator& o = *m_orchestrator;
    if (m_selected < 0 || static_cast<std::size_t>(m_selected) >= o.Size())
        return;
    ServerView& view = m_views[static_cast<std::size_t>(m_selected)];
    DrawLog(m_selected, view);
    DrawPrompt(m_selected, view);
}

void ManagerUi::DrawLog(int i, ServerView& view)
{
    auto& s = m_orchestrator->At(static_cast<std::size_t>(i));
    LogStore& logs = s.session.Logs();
    LogFilter& filter = view.filter;

    auto bit = [](const char* label, unsigned& mask, unsigned value) {
        bool on = (mask & value) != 0;
        if (ImGui::Checkbox(label, &on))
            mask = on ? mask | value : mask & ~value;
        ImGui::SameLine();
    };
    bit("Error", filter.levels, LevelBit(LogLevel::Error));
    bit("Warn", filter.levels, LevelBit(LogLevel::Warn));
    bit("Info", filter.levels, LevelBit(LogLevel::Info));
    bit("Note", filter.levels, LevelBit(LogLevel::Note));
    ImGui::TextDisabled("|");
    ImGui::SameLine();
    bit("Logs", filter.kinds, KindBit(EntryKind::Log));
    bit("Output", filter.kinds, KindBit(EntryKind::Output));
    unsigned prompt = filter.kinds & (KindBit(EntryKind::Command) | KindBit(EntryKind::Result));
    bit("Prompt", prompt, KindBit(EntryKind::Command) | KindBit(EntryKind::Result));
    filter.kinds = (filter.kinds & ~(KindBit(EntryKind::Command) | KindBit(EntryKind::Result))) | prompt;
    bit("Events", filter.kinds, KindBit(EntryKind::Event));
    ImGui::NewLine();
    const float field = ImGui::GetFontSize() * 14.0f;
    ImGui::SetNextItemWidth(field);
    ImGui::InputTextWithHint("##search", "search", &filter.search);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(field * 0.7f);
    ImGui::InputTextWithHint("##func", "function", &filter.func);
    ImGui::SameLine();
    ImGui::Checkbox("Follow", &view.follow);
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
        logs.Clear();
    ImGui::SameLine();
    ImGui::TextDisabled("%zu of %zu", view.view.Seqs().size(), logs.Size());

    view.view.Refresh(logs, filter);
    UpdateRows(logs, view);

    const float promptHeight = ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("log", ImVec2(0, -promptHeight), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
    // scrolled up by the user: stop following; back at the bottom: follow again
    if (ImGui::GetIO().MouseWheel > 0.0f && ImGui::IsWindowHovered())
        view.follow = false;
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(view.rows.size()));
    // fixed columns: time, level, then the text (SameLine's spacing is not in CalcTextSize, so place each one)
    const float gap = ImGui::GetStyle().ItemSpacing.x * 2.0f;
    const float levelX = ImGui::GetCursorPosX() + ImGui::CalcTextSize("00:00:00").x + gap;
    const float textX = levelX + ImGui::CalcTextSize("ERROR").x + gap;
    while (clipper.Step()) {
        for (int r = clipper.DisplayStart; r < clipper.DisplayEnd; ++r) {
            const auto [seq, line] = view.rows[static_cast<std::size_t>(r)];
            const LogEntry* entry = logs.Find(seq);
            if (!entry) {
                ImGui::TextUnformatted("");
                continue;
            }
            const std::string text = EntryText(*entry);
            const std::string_view part = Line(text, line);
            ImGui::PushID(r);
            if (line == 0) {
                const std::string& time = entry->log.time;
                ImGui::TextDisabled("%s", time.size() >= 19 ? time.substr(11, 8).c_str() : time.c_str());
                ImGui::SameLine(levelX);
                ImGui::TextColored(LevelColor(entry->log.level), "%s", LevelTag(entry->log.level));
                ImGui::SameLine(textX);
            } else {
                ImGui::Dummy(ImVec2(0, 0));
                ImGui::SameLine(textX);
            }
            const bool dim = entry->kind == EntryKind::Event;
            const bool command = entry->kind == EntryKind::Command;
            if (dim)
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
            else if (command)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.80f, 0.95f, 1.0f));
            else if (entry->log.level == LogLevel::Error || entry->log.level == LogLevel::Warn)
                ImGui::PushStyleColor(ImGuiCol_Text, LevelColor(entry->log.level));
            else
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Text));
            ImGui::TextUnformatted(part.data(), part.data() + part.size());
            ImGui::PopStyleColor();
            if (entry->kind == EntryKind::Log && !entry->log.func.empty() && ImGui::BeginItemTooltip()) {
                ImGui::Text("%s  [%s]", entry->log.func.c_str(), entry->log.stream.c_str());
                if (!entry->log.file.empty())
                    ImGui::TextDisabled("%s:%d", entry->log.file.c_str(), entry->log.line);
                ImGui::TextUnformatted(entry->log.time.c_str());
                ImGui::EndTooltip();
            }
            if (ImGui::BeginPopupContextItem("entry")) {
                if (ImGui::MenuItem("Copy"))
                    ImGui::SetClipboardText((entry->log.time + " " + LevelTag(entry->log.level) + " " + text).c_str());
                if (entry->kind == EntryKind::Log && !entry->log.func.empty() && ImGui::MenuItem("Only this function"))
                    filter.func = entry->log.func;
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }
    }
    if (!view.follow && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 1.0f && ImGui::GetIO().MouseWheel < 0.0f)
        view.follow = true;
    if (view.follow)
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();
}

void ManagerUi::DrawPrompt(int i, ServerView& view)
{
    auto& s = m_orchestrator->At(static_cast<std::size_t>(i));
    const bool connected = s.session.Link() == LinkState::Connected;
    ImGui::BeginDisabled(!connected);
    ImGui::SetNextItemWidth(-ImGui::GetFontSize() * 4.0f);
    if (view.focusInput) {
        ImGui::SetKeyboardFocusHere();
        view.focusInput = false;
    }
    std::pair<std::vector<std::string>*, int*> history{&view.history, &view.historyPos};
    const bool enter = ImGui::InputTextWithHint(
        "##lua", connected ? "Lua (Enter to run, Up/Down for the history)" : "not connected", &view.input,
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory, HistoryCallback, &history);
    ImGui::SameLine();
    const bool run = ImGui::Button("Run");
    ImGui::EndDisabled();
    if ((enter || run) && !view.input.empty()) {
        if (view.history.empty() || view.history.back() != view.input)
            view.history.push_back(view.input);
        view.historyPos = -1;
        s.session.RunLua(view.input);
        view.input.clear();
        view.follow = true;
        view.focusInput = true;
    }
}

void ManagerUi::DrawQuitDialog()
{
    if (m_quitAsked) {
        ImGui::OpenPopup("Quit");
        m_quitAsked = false;
    }
    if (m_quitting) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::Begin("Stopping", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
        ImGui::TextUnformatted("Stopping the servers started here before quitting...");
        if (ImGui::Button("Quit now (kill them)"))
            m_quit = true;
        ImGui::End();
    }
    if (ImGui::BeginPopupModal("Quit", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted("Servers started by ServerManager stop when it quits.\n"
                               "Attached servers keep running.");
        if (ImGui::Button("Stop them and quit")) {
            m_quitting = true;
            for (std::size_t i = 0; i < m_orchestrator->Size(); ++i)
                if (m_orchestrator->At(i).owned)
                    m_orchestrator->Stop(static_cast<int>(i));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

}  // namespace x2::servermanager
