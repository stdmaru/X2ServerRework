// The UI editor's engine (decision T46, docs/tools/ui-editor.md).

#include "engine.h"

#include "KTDX.h"   // not stdafx.h: Windows would find X2ServerProtocol/StdAfx.h first

#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_timer.h>

#include <algorithm>
#include <cctype>
#include <system_error>

// The engine's globals that the client's executable defines (X2/X2.cpp), set up as its device callbacks do.
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
CKTDGEffectStateManager* g_pEffectStateManager = NULL;

namespace x2::ui_editor {

namespace {

std::string Utf8(const std::wstring& text)
{
    if (text.empty()) {
        return {};
    }
    const int size = ::WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), NULL, 0, NULL, NULL);
    std::string out(static_cast<std::size_t>(std::max(size, 0)), '\0');
    if (size > 0) {
        ::WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), out.data(), size, NULL, NULL);
    }
    return out;
}

std::wstring Wide(const std::string& text)
{
    std::wstring out;
    ConvertUtf8ToWCHAR(out, text.c_str());
    return out;
}

const char* AnchorName(UIAnchor anchor)
{
    switch (anchor) {
    case UIAnchor::Left: return "LEFT";
    case UIAnchor::Right: return "RIGHT";
    case UIAnchor::Stretch: return "STRETCH";
    case UIAnchor::Center: break;
    }
    return "CENTER";
}

const char* TypeName(int type)
{
    static const char* const names[] = {"none",     "button",     "check box", "edit box",   "IME edit box",
                                        "static",   "radio button", "scroll bar", "combo box", "list box",
                                        "slider",   "navigation", "control list", "slot"};
    if (type >= 0 && type < static_cast<int>(std::size(names))) {
        return names[type];
    }
    return "other";
}

// The game globals the dialog and config scripts reach outside KTDXLIB. g_pMain's methods do nothing but install
// fonts (the config scripts); the slot manager's (49 scripts make item slots, 2026-11-14) return objects that accept
// any call, so those scripts still build the rest of their dialog.
constexpr const char* kLuaStubs = R"lua(
local function Sink()
    local sink = {}
    setmetatable(sink, { __index = function() return function() return Sink() end end })
    return sink
end
g_pMain = setmetatable({}, { __index = function(_, key)
    if key == "InstallFont" then
        return function(_, name, file) X2UIEditor_InstallFont(name, file) end
    end
    return function() return nil end
end })
for _, name in ipairs({ "g_pCX2SlotManager", "g_pData", "g_pState", "g_pCouponBox", "g_pMapSelectWindow",
                        "g_pPostBoxInventory" }) do
    _G[name] = Sink()
end
)lua";

void InstallFont(const char* name, const char* file)
{
    g_pKTDXApp->GetDGManager()->GetFontManager()->InstallFont(name != NULL ? name : "", file != NULL ? file : "");
}

// The editor's stage: what CX2State draws and steps for the UI, nothing of the game.
class EditorStage : public CKTDXStage {
public:
    HRESULT OnFrameMove(double time, float elapsed) override
    {
        g_pKTDXApp->GetDGManager()->GetDialogManager()->OnFrameMove(time, elapsed);
        return S_OK;
    }
    HRESULT OnFrameRender() override
    {
        CKTDGManager* manager = g_pKTDXApp->GetDGManager();
        manager->ObjectChainSort();
        manager->ObjectChainNonAlphaRender();
        manager->ObjectChainAlphaRender();
        manager->FrontUIRender();
        return S_OK;
    }
};

}  // namespace

struct Engine::State {
    render::Device& render;
    std::unique_ptr<d3d9::Device> device;
    std::unique_ptr<CKTDXApp> app;
    EditorStage* stage = nullptr;   // the application's once inserted: RemoveStage deletes it
    std::unique_ptr<d3d9::Texture> target;
    struct Open {
        std::string script;
        CKTDGUIDialogType dialog;
        bool loaded = false;
    };
    std::vector<Open> dialogs;
    double time = 0.0;

    explicit State(render::Device& device) : render(device) {}

    ~State()
    {
        for (Open& open : dialogs) {
            if (open.dialog != NULL) {
                app->GetDGManager()->GetDialogManager()->DeleteDlg(open.dialog);
            }
        }
        dialogs.clear();
        if (app) {
            CKTDGStateManager::SetRenderTarget(NULL);
        }
        target.reset();
        if (app) {
            app->RemoveStage(stage, true);
            CKTDGStateManager::InvalidateDeviceObjects();
        }
        app.reset();
        SAFE_RELEASE(g_pEffectStateManager);
        g_pd3dDevice = NULL;
        CKTDGUIAnchorRecorder::SetToolTracking(false);
    }
};

std::unique_ptr<Engine> Engine::Create(render::Device& render, const EngineOptions& options, std::string& error)
{
    if (g_pKTDXApp != NULL) {
        error = "an engine already exists (CKTDXApp is a singleton)";
        return nullptr;
    }
    std::error_code ec;
    std::filesystem::current_path(options.gameFolder, ec);
    if (ec) {
        error = "cannot open the game folder " + options.gameFolder.string() + ": " + ec.message();
        return nullptr;
    }

    SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "x2-none");  // no such driver: the sound manager runs without output
    auto state = std::make_unique<State>(render);
    state->device = std::make_unique<d3d9::Device>(render, nullptr);
    if (!state->device->Ok()) {
        error = "D3D9 device: " + state->device->LastError();
        return nullptr;
    }
    g_pd3dDevice = state->device.get();
    g_pEffectStateManager = new CKTDGEffectStateManager;
    state->app = std::make_unique<CKTDXApp>(nullptr, nullptr, state->device.get());
    CKTDGStateManager::RestoreDeviceObjects();
    CKTDGUIAnchorRecorder::SetToolTracking(true);

    CKTDXDeviceManager* devices = state->app->GetDeviceManager();
    devices->AddHighTexName(L"DLG_");
    devices->AddHighTexName(L"HQ_");
    KGCMassFileManager* files = devices->GetMassFileManager();
    if (options.mountArchives) {
        std::string archiveError;
        if (!files->AddMassFiles(std::filesystem::current_path(), KGCMassFileManager::GetClientArchiveNames(),
                                 archiveError)) {
            error = "cannot load the game data: " + archiveError;
            return nullptr;
        }
    }
    files->LockMassFileMap();

    // CX2Main's start: the enums, then the config script (fonts), with stubs for the game objects.
    lua_State* lua = state->app->GetLuaBinder()->GetLuaState();
    lua_tinker::def(lua, "X2UIEditor_InstallFont", &InstallFont);
    if (luaL_dostring(lua, kLuaStubs) != 0) {
        error = std::string("Lua stubs: ") + lua_tostring(lua, -1);
        return nullptr;
    }
    for (const wchar_t* script : {L"Enum.lua", L"DungeonEnum.lua", L"StringID_def.lua"}) {
        if (files->IsValidFile(script)) {
            state->app->LoadLuaTinker(script);
        }
    }
    InstallFont(FT_DEFAULT_FONTNAME, "Gulim.ttc");     // CX2Main's default font, before the config's
    const wchar_t* config = X2_DEV_TOOLS ? L"Config_INTERNAL.lua" : L"Config_SERVICE.lua";
    if (files->IsValidFile(config)) {
        state->app->LoadLuaTinker(config);
    }

    // The device manager's loading thread starts suspended; CX2Main resumes it once set up. A device reset waits for it.
    devices->ResumeThread();

    state->stage = new EditorStage();
    state->app->InsertStage(state->stage);
    return std::unique_ptr<Engine>(new Engine(std::move(state)));
}

Engine::Engine(std::unique_ptr<State> state) : m_state(std::move(state)) {}

Engine::~Engine() = default;

int Engine::OpenDialog(const std::string& script, std::string& error)
{
    const std::wstring name = Wide(script);
    if (!g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->IsValidFile(name)) {
        error = script + ": no such file in client_data or the archives";
        return -1;
    }
    auto* dialog = new CKTDGUIDialog(m_state->stage, name.c_str(), 0.0f);
    State::Open open{script, dialog, dialog->GetIsSuccessInitOpenScript()};
    g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg(open.dialog);
    if (!open.loaded) {
        error = script + ": the script failed (ErrorLog.txt in the game folder)";
    }
    m_state->dialogs.push_back(open);
    return static_cast<int>(m_state->dialogs.size()) - 1;
}

void Engine::CloseDialog(int index)
{
    if (index < 0 || index >= static_cast<int>(m_state->dialogs.size())) {
        return;
    }
    State::Open& open = m_state->dialogs[static_cast<std::size_t>(index)];
    if (open.dialog != NULL) {
        g_pKTDXApp->GetDGManager()->GetDialogManager()->DeleteDlg(open.dialog);
    }
    m_state->dialogs.erase(m_state->dialogs.begin() + index);
}

void Engine::CloseAll()
{
    while (!m_state->dialogs.empty()) {
        CloseDialog(static_cast<int>(m_state->dialogs.size()) - 1);
    }
}

std::size_t Engine::DialogCount() const
{
    return m_state->dialogs.size();
}

void Engine::SetDialogPos(int index, float x, float y)
{
    if (index >= 0 && index < static_cast<int>(m_state->dialogs.size())) {
        CKTDGUIDialogType& dialog = m_state->dialogs[static_cast<std::size_t>(index)].dialog;
        if (dialog != NULL) {
            dialog->SetPos(D3DXVECTOR2(x, y));
        }
    }
}

void Engine::SetDialogShown(int index, bool shown)
{
    if (index >= 0 && index < static_cast<int>(m_state->dialogs.size())) {
        CKTDGUIDialogType& dialog = m_state->dialogs[static_cast<std::size_t>(index)].dialog;
        if (dialog != NULL) {
            dialog->SetShowEnable(shown, shown);
        }
    }
}

bool Engine::Render(unsigned width, unsigned height, float elapsed, std::string& error)
{
    width = std::max(width, 64u);
    height = std::max(height, 64u);
    State& state = *m_state;
    if (!state.target || state.target->Width() != width || state.target->Height() != height) {
        CKTDGStateManager::SetRenderTarget(NULL);
        state.target = state.device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                                   D3DPOOL_DEFAULT);
        if (!state.target) {
            error = "render target: " + state.device->LastError();
            return false;
        }
        CKTDGStateManager::SetRenderTarget(state.target.get());
        D3DSURFACE_DESC desc{};
        desc.Width = width;
        desc.Height = height;
        desc.Format = D3DFMT_A8R8G8B8;
        state.app->OnResetDevice(&desc);    // the viewport, the UI canvas and the projection follow the new size
    }

    state.time += elapsed;
    state.stage->OnFrameMove(state.time, elapsed);
    if (!state.device->BeginScene()) {
        error = "BeginScene: " + state.device->LastError();
        return false;
    }
    state.app->OnFrameRender();
    state.device->EndScene();
    if (!state.device->Present()) {
        error = "Present: " + state.device->LastError();
        return false;
    }
    return true;
}

render::Texture Engine::Target() const
{
    return m_state->target ? m_state->target->RenderTexture() : render::Texture{};
}

unsigned Engine::TargetWidth() const
{
    return m_state->target ? m_state->target->Width() : 0;
}

unsigned Engine::TargetHeight() const
{
    return m_state->target ? m_state->target->Height() : 0;
}

float Engine::UIScale() const
{
    return g_pKTDXApp->GetResolutionScaleX();
}

float Engine::UIOriginX() const
{
    return g_pKTDXApp->GetUIOrigin().x;
}

float Engine::UIOriginY() const
{
    return g_pKTDXApp->GetUIOrigin().y;
}

std::vector<DialogInfo> Engine::Inspect() const
{
    std::vector<DialogInfo> out;
    const float scale = UIScale();
    const D3DXVECTOR2 origin = g_pKTDXApp->GetUIOrigin();
    for (const State::Open& open : m_state->dialogs) {
        DialogInfo info;
        info.script = open.script;
        info.loaded = open.loaded;
        CKTDGUIDialog* dialog = CKTDGUIDialog::GetObjectByUID(open.dialog.GetHandle());
        if (dialog == nullptr) {
            out.push_back(std::move(info));
            continue;
        }
        info.name = Utf8(dialog->GetName());
        const D3DXVECTOR2 pos = dialog->GetPos();
        const D3DXVECTOR2 canvas = dialog->GetCanvasPos();
        info.pos[0] = pos.x;
        info.pos[1] = pos.y;
        info.canvasPos[0] = canvas.x;
        info.canvasPos[1] = canvas.y;
        info.anchor = AnchorName(dialog->GetAnchor());
        info.anchorDeclared = dialog->IsAnchorDeclared();
        info.placedByCode = dialog->GetPlacedByCode();

        const std::vector<CKTDGUIControl*>& controls = dialog->GetControlList();
        for (std::size_t i = 0; i < controls.size(); ++i) {
            CKTDGUIControl* control = controls[i];
            if (control == NULL) {
                continue;
            }
            ControlInfo c;
            c.index = static_cast<int>(i);
            const std::wstring name = control->GetName() != NULL ? control->GetName() : L"";
            c.named = !name.empty();
            c.name = c.named ? Utf8(name) : "#" + std::to_string(i);
            c.type = static_cast<int>(control->GetType());
            c.typeName = TypeName(c.type);
            c.anchor = AnchorName(control->GetAnchor());
            c.anchorDeclared = control->HasAnchor();
            c.shown = control->GetShow();
            const D3DXVECTOR2 offset = control->GetOffsetPos();
            c.offset[0] = offset.x;
            c.offset[1] = offset.y;
            D3DXVECTOR2 lo, hi;
            c.drawn = control->GetDrawnBounds(lo, hi);
            if (c.drawn) {
                c.drawnMin[0] = lo.x;
                c.drawnMin[1] = lo.y;
                c.drawnMax[0] = hi.x;
                c.drawnMax[1] = hi.y;
                // Layout x relative to the dialog with the control's anchor or stretch, then the canvas.
                const float left = dialog->ControlXToDialogX(control, lo.x);
                const float right = dialog->ControlXToDialogX(control, hi.x);
                c.pixelMin[0] = origin.x + (canvas.x + left) * scale;
                c.pixelMax[0] = origin.x + (canvas.x + right) * scale;
                c.pixelMin[1] = origin.y + (canvas.y + lo.y) * scale;
                c.pixelMax[1] = origin.y + (canvas.y + hi.y) * scale;
            }
            if (c.type == CKTDGUIControl::UCT_STATIC) {
                auto* picturesOf = static_cast<CKTDGUIStatic*>(control);
                for (int p = 0; p < picturesOf->GetPictureNum(); ++p) {
                    CKTDGUIControl::CPictureData* picture = picturesOf->GetPicture(p);
                    if (picture == NULL) {
                        continue;
                    }
                    PictureInfo pi;
                    pi.shown = picture->GetShow();
                    if (picture->pTexture != NULL) {
                        pi.texture = Utf8(picture->pTexture->texName);
                        pi.key = Utf8(picture->pTexture->keyName);
                        pi.textureSize[0] = picture->pTexture->texSize.x;
                        pi.textureSize[1] = picture->pTexture->texSize.y;
                        for (int k = 0; k < 4; ++k) {
                            pi.uv[k][0] = picture->pTexture->uvTexture[k].x;
                            pi.uv[k][1] = picture->pTexture->uvTexture[k].y;
                        }
                    }
                    if (picture->pPoint != NULL) {
                        const D3DXVECTOR2 corners[4] = {picture->pPoint->leftTopPoint, picture->pPoint->rightTopPoint,
                                                        picture->pPoint->leftBottomPoint,
                                                        picture->pPoint->rightBottomPoint};
                        for (int k = 0; k < 4; ++k) {
                            pi.points[k][0] = corners[k].x;
                            pi.points[k][1] = corners[k].y;
                        }
                    }
                    c.pictures.push_back(std::move(pi));
                }
                for (int t = 0; t < picturesOf->GetStringNum(); ++t) {
                    const CKTDGUIControl::UIStringData* string = picturesOf->GetString(t);
                    if (string == NULL) {
                        continue;
                    }
                    StringInfo si;
                    si.text = Utf8(string->msg);
                    si.stringId = string->stringTableID == STR_ID_EMPTY ? -1 : string->stringTableID;
                    si.pos[0] = string->pos.x;
                    si.pos[1] = string->pos.y;
                    si.font = string->fontIndex;
                    si.sortFlag = static_cast<unsigned>(string->sortFlag);
                    si.color[0] = string->color.r;
                    si.color[1] = string->color.g;
                    si.color[2] = string->color.b;
                    si.color[3] = string->color.a;
                    c.strings.push_back(std::move(si));
                }
            }
            info.controls.push_back(std::move(c));
        }
        out.push_back(std::move(info));
    }
    return out;
}

std::vector<std::string> FindDialogScripts(const std::filesystem::path& gameFolder)
{
    std::vector<std::string> out;
    std::error_code ec;
    const std::filesystem::path data = gameFolder / "data";
    for (auto it = std::filesystem::recursive_directory_iterator(
             data, std::filesystem::directory_options::skip_permission_denied, ec);
         !ec && it != std::filesystem::recursive_directory_iterator(); it.increment(ec)) {
        const std::string name = it->path().filename().string();
        if (it->is_directory(ec) && !name.empty() && name[0] == '.') {
            it.disable_recursion_pending();     // client_data's .kom/
            continue;
        }
        if (name.size() < 8 || !it->is_regular_file(ec)) {
            continue;
        }
        std::string lower = name;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char ch) { return std::tolower(ch); });
        if (lower.rfind("dlg_", 0) == 0 && lower.size() > 4 && lower.substr(lower.size() - 4) == ".lua") {
            out.push_back(name);
        }
    }
    std::sort(out.begin(), out.end(), [](const std::string& a, const std::string& b) {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), [](unsigned char x, unsigned char y) {
            return std::tolower(x) < std::tolower(y);
        });
    });
    out.erase(std::unique(out.begin(), out.end()), out.end());
    return out;
}

}  // namespace x2::ui_editor
