// The UI editor's engine (decision T46, docs/tools/ui-editor.md).

#include "engine.h"

#include "KTDX.h"   // not stdafx.h: Windows would find X2ServerProtocol/StdAfx.h first

#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_timer.h>

#include <algorithm>
#include <cctype>
#include <map>
#include <optional>
#include <system_error>
#include <tuple>
#include <unordered_map>

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

std::string Lower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return text;
}

// A script's name without folder, lowercase: how scripts are matched, as the game does.
std::string ScriptKey(const std::string& script)
{
    return Lower(std::filesystem::path(script).filename().string());
}

// What a dialog's scripts placed, before any game state is put into it: its position, and per control the left top
// corner of each picture and the position of each text. An edit's move is the difference of two of these.
struct ScriptLayout {
    D3DXVECTOR2 pos{0, 0};
    std::vector<std::vector<D3DXVECTOR2>> pictures;     // [control][picture]
    std::vector<std::vector<D3DXVECTOR2>> strings;      // [control][string]
};

ScriptLayout CaptureLayout(CKTDGUIDialog& dialog)
{
    ScriptLayout layout;
    layout.pos = dialog.GetPos();
    const std::vector<CKTDGUIControl*>& controls = dialog.GetControlList();
    layout.pictures.resize(controls.size());
    layout.strings.resize(controls.size());
    for (std::size_t c = 0; c < controls.size(); ++c) {
        if (controls[c] == NULL || controls[c]->GetType() != CKTDGUIControl::UCT_STATIC) {
            continue;
        }
        auto* control = static_cast<CKTDGUIStatic*>(controls[c]);
        for (int p = 0; p < control->GetPictureNum(); ++p) {
            const CKTDGUIControl::CPictureData* picture = control->GetPicture(p);
            layout.pictures[c].push_back(picture != NULL && picture->pPoint != NULL ? picture->pPoint->leftTopPoint
                                                                                    : D3DXVECTOR2(0, 0));
        }
        for (int t = 0; t < control->GetStringNum(); ++t) {
            const CKTDGUIControl::UIStringData* string = control->GetString(t);
            layout.strings[c].push_back(string != NULL ? string->pos : D3DXVECTOR2(0, 0));
        }
    }
    return layout;
}

// b - a for one item, (0, 0) when either lacks it.
D3DXVECTOR2 Moved(const std::vector<std::vector<D3DXVECTOR2>>& a, const std::vector<std::vector<D3DXVECTOR2>>& b,
                  std::size_t control, std::size_t item)
{
    if (control >= a.size() || control >= b.size() || item >= a[control].size() || item >= b[control].size()) {
        return D3DXVECTOR2(0, 0);
    }
    return b[control][item] - a[control][item];
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

// Only fonts that exist: the font manager reports a missing one with a modal message box, which on Windows waits for
// a click (and forever in the headless tests, which have no fonts).
void InstallFont(const char* name, const char* file)
{
    if (name == NULL || file == NULL || !g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->IsValidFile(Wide(file))) {
        return;
    }
    g_pKTDXApp->GetDGManager()->GetFontManager()->InstallFont(name, file);
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
        std::vector<std::string> extraScripts;   // loaded into it after the first (LoadScriptInto)
        CKTDGUIDialogType dialog;
        bool loaded = false;
        ScriptLayout layout;                       // what its scripts placed, as last loaded
        std::optional<LiveSnapshot::Dialog> live;  // the game's state last put into it

        // The dialog's scripts, as the live snapshot keys them.
        std::string Key() const
        {
            std::string key = ScriptKey(script);
            for (const std::string& extra : extraScripts) {
                key += '+' + ScriptKey(extra);
            }
            return key;
        }
    };
    std::vector<Open> dialogs;
    double time = 0.0;
    // Script edits not saved (SetScriptText), by ScriptKey.
    std::unordered_map<std::string, std::string> scriptTexts;
    // What each set of scripts placed when first loaded this run (Open::Key): the game, started before the edits,
    // drew that, so the game's state put into a dialog is moved by what the edits moved since (ApplyLive).
    std::map<std::string, ScriptLayout> firstLayouts;

    void NoteLoaded(Open& open)
    {
        if (CKTDGUIDialog* dialog = CKTDGUIDialog::GetObjectByUID(open.dialog.GetHandle()); dialog != nullptr) {
            open.layout = CaptureLayout(*dialog);
            firstLayouts.try_emplace(open.Key(), open.layout);
        }
    }

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
        CKTDXApp::s_fnScriptOverride = nullptr;
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
    State* raw = state.get();
    CKTDXApp::s_fnScriptOverride = [raw](const WCHAR* fileName) -> const std::string* {
        if (raw->scriptTexts.empty() || fileName == nullptr) {
            return nullptr;
        }
        const auto it = raw->scriptTexts.find(ScriptKey(Utf8(fileName)));
        return it != raw->scriptTexts.end() ? &it->second : nullptr;
    };

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
#ifdef X2_DEBUG
    const wchar_t* config = L"Config_INTERNAL.lua";
#else
    const wchar_t* config = L"Config_SERVICE.lua";
#endif
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
    State::Open open{script, {}, dialog, dialog->GetIsSuccessInitOpenScript(), {}, {}};
    g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg(open.dialog);
    if (!open.loaded) {
        error = script + ": the script failed (ErrorLog.txt in the game folder)";
    }
    m_state->NoteLoaded(open);
    m_state->dialogs.push_back(std::move(open));
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

const std::string& Engine::DialogScript(int index) const
{
    return m_state->dialogs.at(static_cast<std::size_t>(index)).script;
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

bool Engine::LoadScriptInto(int index, const std::string& script, std::string& error)
{
    if (index < 0 || index >= static_cast<int>(m_state->dialogs.size())) {
        return false;
    }
    State::Open& open = m_state->dialogs[static_cast<std::size_t>(index)];
    const std::wstring name = Wide(script);
    if (open.dialog == NULL || !g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->IsValidFile(name)) {
        error = script + ": no such file in client_data or the archives";
        return false;
    }
    open.extraScripts.push_back(script);
    const bool ok = open.dialog->OpenScriptFile(name.c_str());
    m_state->NoteLoaded(open);
    if (!ok) {
        error = script + ": the script failed (ErrorLog.txt in the game folder)";
        return false;
    }
    return true;
}

void Engine::SetScriptText(const std::string& script, const std::string* text)
{
    if (text != nullptr) {
        m_state->scriptTexts[ScriptKey(script)] = *text;
    } else {
        m_state->scriptTexts.erase(ScriptKey(script));
    }
}

bool Engine::ScriptText(const std::string& script, std::string& text, std::string& error) const
{
    if (const auto it = m_state->scriptTexts.find(ScriptKey(script)); it != m_state->scriptTexts.end()) {
        text = it->second;
        return true;
    }
    const auto info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile(Wide(script));
    if (info == NULL || info->pRealData == NULL || info->size <= 0) {
        error = script + ": no such file in client_data or the archives";
        return false;
    }
    text.assign(info->pRealData, static_cast<std::size_t>(info->size));
    if (!text.empty() && text[0] == '\033') {
        error = script + " is compiled Lua: edit its source in the client_data";
        return false;
    }
    return true;
}

bool Engine::DialogUses(int index, const std::string& script) const
{
    if (index < 0 || index >= static_cast<int>(m_state->dialogs.size())) {
        return false;
    }
    const State::Open& open = m_state->dialogs[static_cast<std::size_t>(index)];
    const std::string key = ScriptKey(script);
    return ScriptKey(open.script) == key ||
           std::any_of(open.extraScripts.begin(), open.extraScripts.end(),
                       [&](const std::string& extra) { return ScriptKey(extra) == key; });
}

bool Engine::ReloadDialog(int index, std::string& error)
{
    if (index < 0 || index >= static_cast<int>(m_state->dialogs.size())) {
        return false;
    }
    State::Open& open = m_state->dialogs[static_cast<std::size_t>(index)];
    CKTDGUIDialogManager* manager = g_pKTDXApp->GetDGManager()->GetDialogManager();
    D3DXVECTOR2 drawnPos(0, 0);
    bool shown = true;
    if (CKTDGUIDialog* old = CKTDGUIDialog::GetObjectByUID(open.dialog.GetHandle()); old != nullptr) {
        drawnPos = old->GetPos();
        shown = old->GetShow();
    }
    const D3DXVECTOR2 scriptPos = open.layout.pos;
    if (open.dialog != NULL) {
        manager->DeleteDlg(open.dialog);
    }
    const std::wstring name = Wide(open.script);
    auto* dialog = new CKTDGUIDialog(m_state->stage, name.c_str(), 0.0f);
    open.dialog = dialog;
    open.loaded = dialog->GetIsSuccessInitOpenScript();
    manager->AddDlg(open.dialog);
    for (const std::string& extra : open.extraScripts) {
        const std::wstring extraName = Wide(extra);
        open.loaded = dialog->OpenScriptFile(extraName.c_str()) && open.loaded;
    }
    open.layout = CaptureLayout(*dialog);
    if (open.live) {
        const LiveSnapshot::Dialog live = *open.live;
        ApplyLive(index, live);
    } else {
        dialog->SetPos(drawnPos + (open.layout.pos - scriptPos));
        dialog->SetShowEnable(shown, shown);
    }
    if (!open.loaded) {
        error = open.script + ": the script failed (ErrorLog.txt in the game folder)";
        return false;
    }
    return true;
}

const std::vector<std::string>& Engine::DialogExtraScripts(int index) const
{
    return m_state->dialogs.at(static_cast<std::size_t>(index)).extraScripts;
}

namespace {

// A picture as the game holds it: texture and key, corners, texture coordinates, color, shown.
void ApplyPicture(CKTDGUIControl::CPictureData& picture, const LiveSnapshot::Picture& live)
{
    picture.SetShow(live.shown);
    if (!live.texture.empty()) {
        const std::wstring texture = Wide(live.texture), key = Wide(live.key);
        if (picture.pTexture == NULL || picture.pTexture->texName != texture || picture.pTexture->keyName != key) {
            if (key.empty()) {
                picture.SetTex(texture.c_str());
            } else {
                picture.SetTex(texture.c_str(), key.c_str());
            }
        }
    }
    if (!live.full) {
        return;
    }
    if (picture.pPoint == NULL) {
        picture.SetPoint(new CKTDGUIControl::UIPointData());
    }
    CKTDGUIControl::UIPointData& point = *picture.pPoint;
    point.bUseTextureSize = false;
    D3DXVECTOR2* corners[4] = {&point.leftTopPoint, &point.rightTopPoint, &point.leftBottomPoint, &point.rightBottomPoint};
    for (int k = 0; k < 4; ++k) {
        *corners[k] = D3DXVECTOR2(live.points[k][0], live.points[k][1]);
    }
    point.color = D3DXCOLOR(live.color[0], live.color[1], live.color[2], live.color[3]);
    if (picture.pTexture != NULL) {
        for (int k = 0; k < 4; ++k) {
            picture.pTexture->uvTexture[k] = D3DXVECTOR2(live.uv[k][0], live.uv[k][1]);
        }
    }
}

// A static's pictures and texts as the game holds them: what code removed goes, what it added comes.
void ApplyStatic(CKTDGUIStatic& control, const LiveSnapshot::Dialog& live, int index)
{
    int pictures = 0;
    for (const LiveSnapshot::Picture& p : live.pictures) {
        if (p.control == index) {
            pictures = std::max(pictures, p.index + 1);
        }
    }
    while (control.GetPictureNum() > pictures) {
        control.RemovePictureByIndex(control.GetPictureNum() - 1);
    }
    while (control.GetPictureNum() < pictures) {
        auto* picture = new CKTDGUIControl::CPictureData();
        picture->SetShow(false);
        control.AddPicture(picture);
    }
    for (const LiveSnapshot::Picture& p : live.pictures) {
        if (p.control == index) {
            if (CKTDGUIControl::CPictureData* picture = control.GetPicture(p.index); picture != NULL) {
                ApplyPicture(*picture, p);
            }
        }
    }

    int texts = 0;
    for (const LiveSnapshot::Text& t : live.texts) {
        if (t.control == index) {
            texts = std::max(texts, t.index + 1);
        }
    }
    if (control.GetStringNum() != texts) {
        control.RemoveAllString();
        for (int t = 0; t < texts; ++t) {
            control.AddString(new CKTDGUIControl::UIStringData());
        }
    }
    for (const LiveSnapshot::Text& t : live.texts) {
        if (t.control != index) {
            continue;
        }
        CKTDGUIControl::UIStringData* string = control.GetString(t.index);
        if (string == NULL) {
            continue;
        }
        if (t.full) {
            string->fontIndex = t.font;
            string->sortFlag = t.sortFlag;
            string->pos = D3DXVECTOR2(t.pos[0], t.pos[1]);
            string->color = D3DXCOLOR(t.color[0], t.color[1], t.color[2], t.color[3]);
        }
        const std::wstring text = Wide(t.text);
        if (string->msg != text) {
            control.SetString(t.index, text.c_str());
        }
    }
}

}  // namespace

void Engine::ApplyLive(int index, const LiveSnapshot::Dialog& live)
{
    if (index < 0 || index >= static_cast<int>(m_state->dialogs.size())) {
        return;
    }
    State::Open& open = m_state->dialogs[static_cast<std::size_t>(index)];
    CKTDGUIDialogType& dialog = open.dialog;
    if (dialog == NULL) {
        return;
    }
    open.live = live;
    // The game drew what the scripts placed when it started; what the edits moved since moves with it.
    const auto first = m_state->firstLayouts.find(open.Key());
    const ScriptLayout& before = first != m_state->firstLayouts.end() ? first->second : open.layout;
    dialog->SetShowEnable(true, true);
    // The game's anchor before its position, which is in layout units under it: code anchors some dialogs at run time
    // (the quick slot items follow the slot frames), so UIAnchors.lua alone does not say (2026-11-14).
    if (live.anchor >= 0 && live.anchor <= static_cast<int>(UIAnchor::Stretch)) {
        dialog->SetAnchor(static_cast<UIAnchor>(live.anchor));
    }
    dialog->SetPos(D3DXVECTOR2(live.pos[0], live.pos[1]) + (open.layout.pos - before.pos));
    if (dialog->GetLayer() != live.layer) {
        dialog->SetLayer(live.layer);
    }
    if (dialog->GetFront() != live.front) {
        dialog->SetFront(live.front);
    }
    // Controls code added after the scripts: statics, rebuilt from what they hold; the others are left out (an
    // empty static keeps the indexes of those after them).
    for (const LiveSnapshot::Control& c : live.controls) {
        while (c.index >= static_cast<int>(dialog->GetControlList().size())) {
            auto* added = new CKTDGUIStatic();
            added->SetShow(false);
            dialog->AddControl(added);
        }
    }
    const std::vector<CKTDGUIControl*>& controls = dialog->GetControlList();
    for (const LiveSnapshot::Control& c : live.controls) {
        CKTDGUIControl* control = c.index >= 0 ? controls[static_cast<std::size_t>(c.index)] : nullptr;
        if (control == nullptr) {
            continue;
        }
        const bool sameType = c.type < 0 || static_cast<int>(control->GetType()) == c.type;
        control->SetShow(c.shown && sameType);
        if (c.anchor >= 0 && c.anchor <= static_cast<int>(UIAnchor::Stretch)) {
            control->SetAnchor(static_cast<UIAnchor>(c.anchor));
        }
        if (c.hasOffset) {
            control->SetOffsetPos(D3DXVECTOR2(c.offset[0], c.offset[1]));
        }
        if (sameType && control->GetType() == CKTDGUIControl::UCT_STATIC) {
            auto* picturesOf = static_cast<CKTDGUIStatic*>(control);
            ApplyStatic(*picturesOf, live, c.index);
            // Only where the game's corners and positions replaced the script's (full records).
            const auto ci = static_cast<std::size_t>(c.index);
            for (const LiveSnapshot::Picture& p : live.pictures) {
                CKTDGUIControl::CPictureData* picture = p.control == c.index && p.full ? picturesOf->GetPicture(p.index) : NULL;
                const D3DXVECTOR2 moved = Moved(before.pictures, open.layout.pictures, ci, static_cast<std::size_t>(p.index));
                if (picture != NULL && picture->pPoint != NULL && (moved.x != 0 || moved.y != 0)) {
                    picture->pPoint->leftTopPoint += moved;
                    picture->pPoint->rightTopPoint += moved;
                    picture->pPoint->leftBottomPoint += moved;
                    picture->pPoint->rightBottomPoint += moved;
                }
            }
            for (const LiveSnapshot::Text& t : live.texts) {
                CKTDGUIControl::UIStringData* string = t.control == c.index && t.full ? picturesOf->GetString(t.index) : NULL;
                if (string != NULL) {
                    string->pos += Moved(before.strings, open.layout.strings, ci, static_cast<std::size_t>(t.index));
                }
            }
        }
    }
}

void Engine::ApplyAnchors(int index, const AnchorEntry& entry, const AnchorEntry& previous)
{
    if (index < 0 || index >= static_cast<int>(m_state->dialogs.size())) {
        return;
    }
    const State::Open& open = m_state->dialogs[static_cast<std::size_t>(index)];
    CKTDGUIDialog* dialog = CKTDGUIDialog::GetObjectByUID(open.dialog.GetHandle());
    if (dialog == nullptr) {
        return;
    }
    UIAnchor anchor = UIAnchor::Center;
    if (!dialog->IsAnchorFromScript()) {
        if (ParseUIAnchor(entry.dialog.c_str(), anchor)) {
            dialog->SetAnchor(anchor);
        } else if (!previous.dialog.empty()) {
            dialog->ClearAnchor();
        }
    }
    if (!entry.zones.empty() || !previous.zones.empty()) {
        dialog->GetStretch().Clear();
        for (const auto& [lo, hi] : entry.zones) {
            dialog->GetStretch().AddZone(lo, hi);
        }
    }
    const auto apply = [&](const std::string& name, const std::string& value) {
        CKTDGUIControl* control = dialog->GetControl(Wide(name).c_str());
        if (control == nullptr || control->IsAnchorFromScript()) {
            return;
        }
        if (ParseUIAnchor(value.c_str(), anchor)) {
            control->SetAnchor(anchor);
        } else {
            control->ClearAnchor();
        }
    };
    for (const auto& [name, value] : previous.controls) {
        if (entry.Control(name).empty()) {
            apply(name, {});
        }
    }
    for (const auto& [name, value] : entry.controls) {
        apply(name, value);
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
        info.anchorFromScript = dialog->IsAnchorFromScript();
        info.stretchZones = dialog->GetStretch().GetZones();
        info.placedByCode = dialog->GetPlacedByCode();

        // The script calls the dialog noted while it loaded, and how many objects each call set.
        using Call = CKTDGUIDialog::ScriptCall;
        using Kind = CKTDGUIDialog::ScriptCallKind;
        const std::vector<Call>& calls = dialog->GetScriptCalls();
        const std::vector<std::wstring>& scripts = dialog->GetOpenedScripts();
        std::map<std::tuple<unsigned, int, Kind>, int> uses;
        for (const Call& call : calls) {
            ++uses[{call.uScript, call.iLine, call.eKind}];
        }
        const auto ref = [&](const Call& call) {
            SourceRef r;
            r.script = call.uScript < scripts.size() ? Utf8(scripts[call.uScript]) : std::string();
            r.line = call.iLine;
            r.kind = call.eKind == Kind::DialogPos ? CallKind::DialogPos
                     : call.eKind == Kind::String  ? CallKind::String
                                                   : CallKind::Point;
            r.shared = uses[{call.uScript, call.iLine, call.eKind}];
            return r;
        };
        // The objects a call set, by what lua_tinker held: a control may be held as its own class.
        std::unordered_multimap<const void*, const Call*> callsOf;
        for (const Call& call : calls) {
            callsOf.emplace(call.pObject, &call);
        }
        const auto callsFor = [&](const void* object, Kind kind) {
            std::vector<const Call*> found;
            const auto [begin, end] = callsOf.equal_range(object);
            for (auto it = begin; it != end; ++it) {
                if (it->second->eKind == kind) {
                    found.push_back(it->second);
                }
            }
            std::sort(found.begin(), found.end());  // in call order: the calls are a vector
            return found;
        };
        if (const auto found = callsFor(dialog, Kind::DialogPos); !found.empty()) {
            info.posSource = ref(*found.back());
        }

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
            const void* self = dynamic_cast<const void*>(control);
            for (const void* object : {static_cast<const void*>(control), self}) {
                for (const Call* call : callsFor(object, Kind::Point)) {
                    c.points.push_back(ref(*call));
                }
                if (self == static_cast<const void*>(control)) {
                    break;
                }
            }
            c.anchor = AnchorName(control->GetAnchor());
            c.anchorDeclared = control->HasAnchor();
            c.anchorFromScript = control->IsAnchorFromScript();
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
                    if (const auto found = callsFor(picture, Kind::Point); !found.empty()) {
                        pi.point = ref(*found.back());
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
                const std::vector<const Call*> added = callsFor(picturesOf, Kind::String);
                for (int t = 0; t < picturesOf->GetStringNum(); ++t) {
                    const CKTDGUIControl::UIStringData* string = picturesOf->GetString(t);
                    if (string == NULL) {
                        continue;
                    }
                    StringInfo si;
                    if (static_cast<std::size_t>(t) < added.size()) {
                        si.source = ref(*added[static_cast<std::size_t>(t)]);
                    }
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

std::filesystem::path FindLooseScript(const std::filesystem::path& gameFolder, const std::string& script)
{
    const std::string key = ScriptKey(script);
    std::error_code ec;
    for (auto it = std::filesystem::recursive_directory_iterator(
             gameFolder / "data", std::filesystem::directory_options::skip_permission_denied, ec);
         !ec && it != std::filesystem::recursive_directory_iterator(); it.increment(ec)) {
        const std::string name = it->path().filename().string();
        if (it->is_directory(ec) && !name.empty() && name[0] == '.') {
            it.disable_recursion_pending();     // client_data's .kom/
            continue;
        }
        if (Lower(name) == key && it->is_regular_file(ec)) {
            return it->path();
        }
    }
    return {};
}

}  // namespace x2::ui_editor
