// x2_ui_editor: the client's dialogs drawn by the client's own GUI code, at any resolution and aspect, to inspect (and,
// in later slices, edit) anchors, positions, textures and UV. Decision T46; docs/tools/ui-editor.md.
//
//   x2_ui_editor [<game folder>] [--open DLG_x.lua]... [--size WxH]
//
// The game folder holds the .kom archives and, in dev builds, data/ (a client_data); the default is the working folder.

#include "engine.h"
#include "record.h"

#include "x2/app/imgui_layer.h"
#include "x2/render/sdlgpu.h"

#include <imgui.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

namespace {

using x2::ui_editor::ControlInfo;
using x2::ui_editor::DialogInfo;

struct Aspect {
    const char* name;
    float ratio;
};
constexpr Aspect kAspects[] = {{"4:3", 4.0f / 3.0f}, {"16:10", 16.0f / 10.0f}, {"16:9", 16.0f / 9.0f},
                               {"21:9", 64.0f / 27.0f}};
constexpr unsigned kHeights[] = {720, 768, 900, 1050, 1080, 1200, 1440, 2160};

// Width for a height and aspect, even, as the standard modes are (1366 is the one odd case and 1365 would do).
unsigned WidthFor(unsigned height, float ratio)
{
    const unsigned width = static_cast<unsigned>(std::lround(static_cast<double>(height) * ratio));
    return width + (width & 1u);
}

struct Selection {
    int dialog = -1;
    int control = -1;   // index in DialogInfo::controls
};

struct EditorState {
    std::filesystem::path gameFolder;
    std::vector<std::string> scripts;
    x2::ui_editor::Record record;
    char filter[128] = {};
    int aspect = 2;     // 16:9
    int height = 4;     // 1080
    bool fit = true;
    bool showBounds = false;
    Selection selected;
    std::vector<DialogInfo> inspected;
    std::string message;
};

bool ContainsNoCase(const std::string& text, const char* needle)
{
    if (needle[0] == '\0') {
        return true;
    }
    const auto lower = [](unsigned char c) { return static_cast<char>(std::tolower(c)); };
    std::string a = text, b = needle;
    std::transform(a.begin(), a.end(), a.begin(), lower);
    std::transform(b.begin(), b.end(), b.begin(), lower);
    return a.find(b) != std::string::npos;
}

void OpenScript(x2::ui_editor::Engine& engine, EditorState& state, const std::string& script)
{
    std::string error;
    const int index = engine.OpenDialog(script, error);
    if (index < 0) {
        state.message = error;
        return;
    }
    state.message = error.empty() ? "opened " + script : error;
    if (const auto* positions = state.record.Positions(script); positions != nullptr && !positions->empty()) {
        engine.SetDialogPos(index, positions->front().x, positions->front().y);
    }
    state.selected = {index, -1};
}

void DialogsPanel(x2::ui_editor::Engine& engine, EditorState& state)
{
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::InputTextWithHint("##filter", "filter dialogs", state.filter, sizeof(state.filter));
    ImGui::TextDisabled("double-click opens, Ctrl adds to the open ones");
    if (ImGui::BeginChild("scripts", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.6f), ImGuiChildFlags_Borders)) {
        for (const std::string& script : state.scripts) {
            if (!ContainsNoCase(script, state.filter)) {
                continue;
            }
            const bool recorded = state.record.Positions(script) != nullptr;
            if (!recorded) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
            }
            ImGui::Selectable(script.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
            if (!recorded) {
                ImGui::PopStyleColor();
            }
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if (!ImGui::GetIO().KeyCtrl) {
                    engine.CloseAll();
                }
                OpenScript(engine, state, script);
            }
        }
    }
    ImGui::EndChild();

    ImGui::SeparatorText("Open");
    for (int i = 0; i < static_cast<int>(state.inspected.size()); ++i) {
        const DialogInfo& dialog = state.inspected[static_cast<std::size_t>(i)];
        ImGui::PushID(i);
        if (ImGui::SmallButton("x")) {
            engine.CloseDialog(i);
            state.selected = {};
            ImGui::PopID();
            break;
        }
        ImGui::SameLine();
        if (ImGui::Selectable(dialog.script.c_str(), state.selected.dialog == i && state.selected.control < 0)) {
            state.selected = {i, -1};
        }
        ImGui::PopID();
    }
    if (!state.inspected.empty() && ImGui::Button("Close all")) {
        engine.CloseAll();
        state.selected = {};
    }
}

void PictureTable(const ControlInfo& control)
{
    if (control.pictures.empty()) {
        return;
    }
    ImGui::SeparatorText("Pictures");
    for (std::size_t p = 0; p < control.pictures.size(); ++p) {
        const auto& picture = control.pictures[p];
        ImGui::PushID(static_cast<int>(p));
        const std::string label = std::to_string(p + 1) + ": " + (picture.texture.empty() ? "(no texture)" : picture.texture) +
                                  (picture.key.empty() ? "" : " / " + picture.key);
        if (ImGui::TreeNode(label.c_str())) {
            ImGui::Text("shown: %s", picture.shown ? "yes" : "no");
            ImGui::Text("texture size: %.0f x %.0f", static_cast<double>(picture.textureSize[0]),
                        static_cast<double>(picture.textureSize[1]));
            static const char* const corners[] = {"left top", "right top", "left bottom", "right bottom"};
            if (ImGui::BeginTable("corners", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit)) {
                ImGui::TableSetupColumn("corner");
                ImGui::TableSetupColumn("point");
                ImGui::TableSetupColumn("UV (texels)");
                ImGui::TableHeadersRow();
                for (int k = 0; k < 4; ++k) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(corners[k]);
                    ImGui::TableNextColumn();
                    ImGui::Text("%.1f, %.1f", static_cast<double>(picture.points[k][0]),
                                static_cast<double>(picture.points[k][1]));
                    ImGui::TableNextColumn();
                    ImGui::Text("%.1f, %.1f", static_cast<double>(picture.uv[k][0] * picture.textureSize[0]),
                                static_cast<double>(picture.uv[k][1] * picture.textureSize[1]));
                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}

void InspectorPanel(x2::ui_editor::Engine& engine, EditorState& state)
{
    const Selection& sel = state.selected;
    if (sel.dialog < 0 || sel.dialog >= static_cast<int>(state.inspected.size())) {
        ImGui::TextDisabled("Open a dialog, then click a control on the screen.");
        return;
    }
    const DialogInfo& dialog = state.inspected[static_cast<std::size_t>(sel.dialog)];
    ImGui::SeparatorText("Dialog");
    ImGui::Text("script: %s", dialog.script.c_str());
    ImGui::Text("name: %s", dialog.name.c_str());
    if (!dialog.loaded) {
        ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "the script failed: see ErrorLog.txt");
    }
    ImGui::Text("anchor: %s%s", dialog.anchor.c_str(), dialog.anchorDeclared ? "" : " (default)");
    ImGui::Text("position: %.0f, %.0f (drawn at %.0f, %.0f)", static_cast<double>(dialog.pos[0]),
                static_cast<double>(dialog.pos[1]), static_cast<double>(dialog.canvasPos[0]),
                static_cast<double>(dialog.canvasPos[1]));
    if (const auto* positions = state.record.Positions(dialog.script); positions != nullptr) {
        const auto& r = state.record.Of(dialog.script);
        ImGui::Text("recorded: %zu position(s)%s", positions->size(), r.placedByCode ? ", placed by code" : "");
        for (std::size_t i = 0; i < positions->size(); ++i) {
            const auto& p = (*positions)[i];
            ImGui::PushID(static_cast<int>(i));
            char label[64];
            std::snprintf(label, sizeof(label), "%.0f, %.0f", static_cast<double>(p.x), static_cast<double>(p.y));
            if (ImGui::SmallButton(label)) {
                engine.SetDialogPos(sel.dialog, p.x, p.y);
            }
            ImGui::PopID();
            if (i + 1 < positions->size()) {
                ImGui::SameLine();
            }
        }
    } else {
        ImGui::TextDisabled("not recorded: drawn at the script's position");
    }

    ImGui::SeparatorText("Controls");
    if (ImGui::BeginChild("controls", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.4f), ImGuiChildFlags_Borders)) {
        for (int c = 0; c < static_cast<int>(dialog.controls.size()); ++c) {
            const ControlInfo& control = dialog.controls[static_cast<std::size_t>(c)];
            char label[256];
            std::snprintf(label, sizeof(label), "%s  [%s]%s%s", control.name.c_str(), control.typeName.c_str(),
                          control.anchorDeclared ? "  " : "", control.anchorDeclared ? control.anchor.c_str() : "");
            ImGui::PushID(c);
            if (!control.drawn) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
            }
            if (ImGui::Selectable(label, sel.control == c)) {
                state.selected.control = c;
            }
            if (!control.drawn) {
                ImGui::PopStyleColor();
            }
            ImGui::PopID();
        }
    }
    ImGui::EndChild();

    if (sel.control < 0 || sel.control >= static_cast<int>(dialog.controls.size())) {
        return;
    }
    const ControlInfo& control = dialog.controls[static_cast<std::size_t>(sel.control)];
    ImGui::SeparatorText("Control");
    ImGui::Text("name: %s%s", control.name.c_str(), control.named ? "" : " (unnamed)");
    ImGui::Text("type: %s", control.typeName.c_str());
    ImGui::Text("anchor: %s%s", control.anchor.c_str(), control.anchorDeclared ? "" : " (the dialog's)");
    ImGui::Text("shown: %s", control.shown ? "yes" : "no");
    ImGui::Text("offset: %.0f, %.0f", static_cast<double>(control.offset[0]), static_cast<double>(control.offset[1]));
    if (control.drawn) {
        ImGui::Text("drawn: %.0f, %.0f to %.0f, %.0f (%.0f x %.0f)", static_cast<double>(control.drawnMin[0]),
                    static_cast<double>(control.drawnMin[1]), static_cast<double>(control.drawnMax[0]),
                    static_cast<double>(control.drawnMax[1]),
                    static_cast<double>(control.drawnMax[0] - control.drawnMin[0]),
                    static_cast<double>(control.drawnMax[1] - control.drawnMin[1]));
    } else {
        ImGui::TextDisabled("drew nothing in the last frame");
    }
    PictureTable(control);
}

// The topmost drawn control under a render target pixel: the last open dialog first, its last control first.
Selection Pick(const std::vector<DialogInfo>& dialogs, float x, float y)
{
    for (int d = static_cast<int>(dialogs.size()) - 1; d >= 0; --d) {
        const auto& controls = dialogs[static_cast<std::size_t>(d)].controls;
        for (int c = static_cast<int>(controls.size()) - 1; c >= 0; --c) {
            const ControlInfo& control = controls[static_cast<std::size_t>(c)];
            if (control.drawn && control.shown && x >= control.pixelMin[0] && x < control.pixelMax[0] &&
                y >= control.pixelMin[1] && y < control.pixelMax[1]) {
                return {d, c};
            }
        }
    }
    return {};
}

void ScreenPanel(x2::ui_editor::Engine& engine, x2::render::Device& device, EditorState& state)
{
    ImGui::SetNextItemWidth(90);
    if (ImGui::BeginCombo("aspect", kAspects[state.aspect].name)) {
        for (int i = 0; i < static_cast<int>(std::size(kAspects)); ++i) {
            if (ImGui::Selectable(kAspects[i].name, state.aspect == i)) {
                state.aspect = i;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    const std::string heightLabel = std::to_string(kHeights[state.height]);
    if (ImGui::BeginCombo("height", heightLabel.c_str())) {
        for (int i = 0; i < static_cast<int>(std::size(kHeights)); ++i) {
            if (ImGui::Selectable(std::to_string(kHeights[i]).c_str(), state.height == i)) {
                state.height = i;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::Text("%u x %u", engine.TargetWidth(), engine.TargetHeight());
    ImGui::SameLine();
    ImGui::Checkbox("fit", &state.fit);
    ImGui::SameLine();
    ImGui::Checkbox("all boxes", &state.showBounds);
    if (!state.message.empty()) {
        ImGui::SameLine();
        ImGui::TextDisabled("%s", state.message.c_str());
    }

    const x2::render::Texture target = engine.Target();
    SDL_GPUTexture* texture = x2::render::sdlgpu::Native::Texture(device, target);
    if (texture == nullptr) {
        return;
    }
    if (ImGui::BeginChild("screen", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
        const float tw = static_cast<float>(engine.TargetWidth()), th = static_cast<float>(engine.TargetHeight());
        const ImVec2 avail = ImGui::GetContentRegionAvail();
        float zoom = 1.0f;
        if (state.fit) {
            zoom = std::min(avail.x / tw, avail.y / th);
        }
        const ImVec2 size(tw * zoom, th * zoom);
        const ImVec2 origin = ImGui::GetCursorScreenPos();
        ImGui::Image(ImTextureRef(static_cast<ImTextureID>(reinterpret_cast<intptr_t>(texture))), size);
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto box = [&](const ControlInfo& c, ImU32 color, float thickness) {
            draw->AddRect(ImVec2(origin.x + c.pixelMin[0] * zoom, origin.y + c.pixelMin[1] * zoom),
                          ImVec2(origin.x + c.pixelMax[0] * zoom, origin.y + c.pixelMax[1] * zoom), color, 0.0f, thickness);
        };
        if (state.showBounds) {
            for (const DialogInfo& dialog : state.inspected) {
                for (const ControlInfo& control : dialog.controls) {
                    if (control.drawn && control.shown) {
                        box(control, IM_COL32(80, 160, 255, 110), 1.0f);
                    }
                }
            }
        }
        if (ImGui::IsItemHovered()) {
            const ImVec2 mouse = ImGui::GetMousePos();
            const float px = (mouse.x - origin.x) / zoom, py = (mouse.y - origin.y) / zoom;
            const Selection hover = Pick(state.inspected, px, py);
            const float lx = (px - engine.UIOriginX()) / engine.UIScale();
            const float ly = (py - engine.UIOriginY()) / engine.UIScale();
            if (hover.dialog >= 0) {
                const ControlInfo& c =
                    state.inspected[static_cast<std::size_t>(hover.dialog)].controls[static_cast<std::size_t>(hover.control)];
                box(c, IM_COL32(255, 255, 255, 160), 1.0f);
                ImGui::SetTooltip("%s\n%s [%s]\nlayout %.0f, %.0f",
                                  state.inspected[static_cast<std::size_t>(hover.dialog)].script.c_str(), c.name.c_str(),
                                  c.typeName.c_str(), static_cast<double>(lx), static_cast<double>(ly));
            } else {
                ImGui::SetTooltip("layout %.0f, %.0f", static_cast<double>(lx), static_cast<double>(ly));
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                state.selected = hover;
            }
        }
        const Selection& sel = state.selected;
        if (sel.dialog >= 0 && sel.dialog < static_cast<int>(state.inspected.size()) && sel.control >= 0 &&
            sel.control < static_cast<int>(state.inspected[static_cast<std::size_t>(sel.dialog)].controls.size())) {
            box(state.inspected[static_cast<std::size_t>(sel.dialog)].controls[static_cast<std::size_t>(sel.control)],
                IM_COL32(255, 200, 0, 255), 2.0f);
        }
    }
    ImGui::EndChild();
}

}  // namespace

int main(int argc, char** argv)
{
    EditorState state;
    state.gameFolder = std::filesystem::current_path();
    std::vector<std::string> toOpen;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--open") == 0 && i + 1 < argc) {
            toOpen.emplace_back(argv[++i]);
        } else if (std::strcmp(argv[i], "--size") == 0 && i + 1 < argc) {
            unsigned w = 0, h = 0;
            if (std::sscanf(argv[++i], "%ux%u", &w, &h) == 2 && h > 0) {
                const float ratio = static_cast<float>(w) / static_cast<float>(h);
                for (int a = 0; a < static_cast<int>(std::size(kAspects)); ++a) {
                    if (std::fabs(kAspects[a].ratio - ratio) < 0.02f) {
                        state.aspect = a;
                    }
                }
                for (int k = 0; k < static_cast<int>(std::size(kHeights)); ++k) {
                    if (kHeights[k] == h) {
                        state.height = k;
                    }
                }
            }
        } else {
            state.gameFolder = argv[i];
        }
    }
    state.gameFolder = std::filesystem::absolute(state.gameFolder);

    std::string error;
    auto sdl = x2::app::Sdl::Init(error);
    if (!sdl) {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }
    x2::app::WindowOptions windowOptions;
    windowOptions.title = "x2 UI editor";
    windowOptions.width = 1600;
    windowOptions.height = 900;
    auto window = x2::app::Window::Create(windowOptions, error);
    if (!window) {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }
    auto device = x2::render::Device::Create({}, error);
    if (!device) {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }
    x2::render::WindowClaim claim(*device, window->Handle());
    if (!claim.Ok()) {
        std::fprintf(stderr, "%s\n", device->LastError().c_str());
        return 1;
    }
    x2::app::ImGuiOptions guiOptions;   // fixed panels: nothing for an ini file to keep
    auto gui = x2::app::ImGuiLayer::Create(*window, *device, guiOptions, error);
    if (!gui) {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }

    x2::ui_editor::EngineOptions engineOptions;
    engineOptions.gameFolder = state.gameFolder;
    auto engine = x2::ui_editor::Engine::Create(*device, engineOptions, error);
    if (!engine) {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }
    state.scripts = x2::ui_editor::FindDialogScripts(state.gameFolder);
    state.record = x2::ui_editor::Record::Load(state.gameFolder / "UIAnchorsRecord.tsv");
    for (const std::string& script : toOpen) {
        OpenScript(*engine, state, script);
    }

    std::uint64_t last = SDL_GetTicksNS();
    bool running = true;
    while (running) {
        running = x2::app::PollEvents([&](const SDL_Event& event) { gui->ProcessEvent(event); }, window->Id());
        if (window->Minimized()) {
            x2::app::WaitForEvents(100);
            continue;
        }
        const std::uint64_t now = SDL_GetTicksNS();
        const float elapsed = std::min(static_cast<float>(static_cast<double>(now - last) / 1e9), 0.1f);
        last = now;

        const unsigned height = kHeights[state.height];
        if (!engine->Render(WidthFor(height, kAspects[state.aspect].ratio), height, elapsed, error)) {
            std::fprintf(stderr, "%s\n", error.c_str());
            break;
        }
        state.inspected = engine->Inspect();
        if (state.selected.dialog >= static_cast<int>(state.inspected.size())) {
            state.selected = {};
        }

        gui->NewFrame();
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const float side = std::max(280.0f, viewport->WorkSize.x * 0.2f);
        const ImGuiWindowFlags fixed = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(ImVec2(side, viewport->WorkSize.y));
        if (ImGui::Begin("Dialogs", nullptr, fixed)) {
            DialogsPanel(*engine, state);
        }
        ImGui::End();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + side, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x - 2 * side, viewport->WorkSize.y));
        if (ImGui::Begin("Screen", nullptr, fixed)) {
            ScreenPanel(*engine, *device, state);
        }
        ImGui::End();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - side, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(side, viewport->WorkSize.y));
        if (ImGui::Begin("Inspector", nullptr, fixed)) {
            InspectorPanel(*engine, state);
        }
        ImGui::End();

        x2::render::CommandBuffer* commands = device->AcquireCommandBuffer();
        x2::render::SwapchainTexture swapchain;
        if (!commands || !commands->WaitAndAcquireSwapchainTexture(window->Handle(), swapchain)) {
            if (commands) {
                commands->Cancel();
            }
            std::fprintf(stderr, "frame: %s\n", device->LastError().c_str());
            break;
        }
        gui->Render(*commands, swapchain.texture, {0.08f, 0.08f, 0.09f, 1.0f});
        commands->Submit();
    }
    engine.reset();
    device->WaitIdle();
    return 0;
}
