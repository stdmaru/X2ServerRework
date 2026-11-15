// x2_ui_editor: the client's dialogs drawn by the client's own GUI code, at any resolution and aspect, to inspect them,
// edit their anchors (UIAnchors.lua), move what their scripts place (the scripts' own numbers), and change a picture's
// texture, key and UV rectangle (the scripts' SetTex and the textures' .tet). Decision T46; docs/tools/ui-editor.md.
//
//   x2_ui_editor [<game folder>] [--open DLG_x.lua]... [--size WxH]
//
// The game folder holds the .kom archives and, in dev builds, data/ (a client_data); the default is the working folder.
// "Watch game" mirrors the UI of a client running there with X2_UI_LIVE=1 (UILive.tsv); "Start game" starts one.

#include "engine.h"
#include "live.h"
#include "record.h"
#include "tet_file.h"

#include "x2/app/imgui_layer.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>  // WinMain on Windows
#include <imgui.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <functional>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

using x2::ui_editor::AnchorEntry;
using x2::ui_editor::ControlInfo;
using x2::ui_editor::DialogInfo;
using x2::ui_editor::PictureInfo;
using x2::ui_editor::SourceRef;
using x2::ui_editor::TetFile;
using x2::ui_editor::TetRect;

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
    // An exact size instead of aspect x height: the game's viewport while mirroring it, so the anchors place the
    // script controls for the same aspect the code positioned the rest for (any window size, 2026-11-14). Choosing an
    // aspect or a height clears it.
    unsigned exact[2] = {};
    bool fit = true;
    float zoom = 1.0f;      // screen pixels per render target pixel when not fitted
    bool showBounds = false;
    Selection selected;
    std::vector<DialogInfo> inspected;
    std::string message;

    std::unordered_map<std::string, std::string> scriptByKey;  // lowercase name -> as listed in `scripts`
    std::vector<std::string> scenes;    // recorded game states (XS_*)
    int scene = 0;                      // 0: every script; n: scenes[n - 1]
    std::set<std::string> sceneScripts;   // lowercase, of the chosen scene
    std::filesystem::file_time_type recordTime{};

    // Watch game: the running client's UILive.tsv, mirrored while checked.
    bool watch = false;
    x2::ui_editor::LiveSnapshot live;
    std::filesystem::file_time_type liveTime{};
    std::uint64_t lastPoll = 0;
    SDL_Process* game = nullptr;        // started by "Start game"

    // UIAnchors.lua, as the inspector edits it. The scripts edited this run get their entry put back into their
    // dialogs every frame, over what opening or watching the game set (the game reads the file when it starts).
    x2::ui_editor::AnchorsFile anchors;
    std::filesystem::path anchorsPath;
    std::set<std::string> anchorsEdited;    // lowercase
    // Dialog scripts as the inspector moved what they place (docs/tools/ui-editor.md#editing-positions), by Key:
    // the text, the file's text when first edited or last saved, and the name as the dialog loaded it.
    std::unordered_map<std::string, std::string> scriptTexts, scriptSaved, scriptNames;
    // Textures' .tet files as the inspector changed their UV rectangles and keys
    // (docs/tools/ui-editor.md#editing-textures-and-uv), by Key of the texture: the bytes, the file's when first
    // edited or last saved, and the texture's name as the scripts give it.
    std::unordered_map<std::string, std::string> tetBytes, tetSaved, tetTextures;
    // The script lines that name a texture's key (FindKeyUses), by "texture|key" lowercase, and the keys whose UV the
    // user chose to change in place although others use them.
    std::unordered_map<std::string, std::vector<x2::ui_editor::KeyUse>> keyUses;
    std::set<std::string> keysInPlace;
    std::set<std::string> noTet;    // textures without a .tet the game reads (Key), not looked for again
    // A shared key's UV about to change: asks whether in place or in a new key (KeyPopup).
    struct KeyAsk {
        bool open = false;
        std::string texture, key, dialog;
        SourceRef texSource;
        std::vector<x2::ui_editor::KeyUse> uses;
        char name[96] = {};
    } keyAsk;

    // Undo and redo of the edits: a script's UIAnchors.lua entry, or the texts of the scripts a move changed, before
    // and after each. Steps of one drag (merge: the dragged item) are one.
    struct Step {
        bool anchors = true;
        std::string script;                 // anchors: the entry's script; texts: the first script changed
        AnchorEntry before, after;
        struct Text {
            std::string script, before, after;  // tet: the texture whose .tet bytes these are
            bool tet = false;
        };
        std::vector<Text> texts;
        unsigned merge = 0;
    };
    std::vector<Step> undo, redo;
    bool quitAsked = false;
    // Dragging the selected control on the screen: the move not applied yet (whole layout units are), and whether it
    // moves the dialog (Alt) instead.
    bool dragging = false;
    bool draggingDialog = false;
    float dragRest[2] = {};

    bool ScriptsChanged() const
    {
        return std::any_of(scriptTexts.begin(), scriptTexts.end(), [this](const auto& text) {
            const auto saved = scriptSaved.find(text.first);
            return saved == scriptSaved.end() || saved->second != text.second;
        });
    }
    bool TetsChanged() const
    {
        return std::any_of(tetBytes.begin(), tetBytes.end(), [this](const auto& bytes) {
            const auto saved = tetSaved.find(bytes.first);
            return saved == tetSaved.end() || saved->second != bytes.second;
        });
    }
    bool Changed() const { return anchors.Changed() || ScriptsChanged() || TetsChanged(); }
};

std::string Key(const std::string& script)
{
    std::string key = std::filesystem::path(script).filename().string();
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return key;
}

void LoadAnchors(EditorState& state)
{
    std::string error;
    state.anchorsPath = x2::ui_editor::AnchorsFile::Find(state.gameFolder);
    state.anchors = x2::ui_editor::AnchorsFile::Load(state.anchorsPath, error);
    state.anchorsEdited.clear();
    state.undo.clear();
    state.redo.clear();
    if (!error.empty()) {
        state.message = error;
    }
}

// Changes a script's entry in UIAnchors.lua and puts it into the open dialogs of that script.
void SetAnchors(x2::ui_editor::Engine& engine, EditorState& state, const std::string& script, const AnchorEntry& entry)
{
    const AnchorEntry previous = state.anchors.Get(script);
    state.anchors.Set(script, entry);
    const std::string key = Key(script);
    state.anchorsEdited.insert(key);
    for (int i = 0; i < static_cast<int>(engine.DialogCount()); ++i) {
        if (Key(engine.DialogScript(i)) == key) {
            engine.ApplyAnchors(i, state.anchors.Get(script), previous);
        }
    }
}

// An edit from the inspector, which undo takes back. merge: an id that joins the steps of one drag (0: none).
void EditAnchors(x2::ui_editor::Engine& engine, EditorState& state, const std::string& script, const AnchorEntry& entry,
                 unsigned merge = 0)
{
    const AnchorEntry before = state.anchors.Get(script);
    if (before == entry) {
        return;
    }
    if (merge != 0 && !state.undo.empty() && state.undo.back().anchors && state.undo.back().merge == merge &&
        state.undo.back().script == script) {
        state.undo.back().after = entry;
    } else {
        EditorState::Step step;
        step.script = script;
        step.before = before;
        step.after = entry;
        step.merge = merge;
        state.undo.push_back(std::move(step));
    }
    state.redo.clear();
    SetAnchors(engine, state, script, entry);
}

// Makes a script's text this one: it runs from it in every dialog loaded after, and the open dialogs built from it
// are built again.
void SetScript(x2::ui_editor::Engine& engine, EditorState& state, const std::string& script, const std::string& text)
{
    const std::string key = Key(script);
    state.scriptTexts[key] = text;
    state.scriptNames.try_emplace(key, script);
    engine.SetScriptText(script, &text);
    std::string error;
    for (int i = 0; i < static_cast<int>(engine.DialogCount()); ++i) {
        if (engine.DialogUses(i, script) && !engine.ReloadDialog(i, error)) {
            state.message = error;
        }
    }
}

// Makes a texture's .tet these bytes in the engine, and builds again the open dialogs with pictures of it.
void SetTet(x2::ui_editor::Engine& engine, EditorState& state, const std::string& texture, const std::string& bytes)
{
    const std::string key = Key(texture);
    state.tetBytes[key] = bytes;
    state.tetTextures.try_emplace(key, texture);
    std::string error;
    if (!engine.SetTet(texture, bytes, error)) {
        state.message = error;
        return;
    }
    for (int i = 0; i < static_cast<int>(engine.DialogCount()); ++i) {
        if (engine.DialogUsesTexture(i, texture) && !engine.ReloadDialog(i, error)) {
            state.message = error;
        }
    }
}

void Undo(x2::ui_editor::Engine& engine, EditorState& state, bool redo)
{
    auto& from = redo ? state.redo : state.undo;
    auto& to = redo ? state.undo : state.redo;
    if (from.empty()) {
        return;
    }
    EditorState::Step step = std::move(from.back());
    from.pop_back();
    step.merge = 0;
    if (step.anchors) {
        SetAnchors(engine, state, step.script, redo ? step.after : step.before);
    } else {
        for (const EditorState::Step::Text& text : step.texts) {
            if (text.tet) {
                SetTet(engine, state, text.script, redo ? text.after : text.before);
            } else {
                SetScript(engine, state, text.script, redo ? text.after : text.before);
            }
        }
    }
    state.message = std::string(redo ? "redone: " : "undone: ") + step.script;
    to.push_back(std::move(step));
}

void ApplyTexts(x2::ui_editor::Engine& engine, EditorState& state, const std::vector<EditorState::Step::Text>& texts,
                unsigned merge);

// A script's text as edited, or its file's (noted as the saved one). False with the message set when it has none.
bool CurrentScript(x2::ui_editor::Engine& engine, EditorState& state, const std::string& script, std::string& text)
{
    const std::string key = Key(script);
    if (const auto it = state.scriptTexts.find(key); it != state.scriptTexts.end()) {
        text = it->second;
        return true;
    }
    std::string error;
    if (!engine.ScriptText(script, text, error)) {
        state.message = error;
        return false;
    }
    state.scriptSaved.try_emplace(key, text);
    return true;
}

// A texture's .tet as edited, or its file's (noted as the saved one). False with the message set when it has none.
bool CurrentTet(x2::ui_editor::Engine& engine, EditorState& state, const std::string& texture, std::string& bytes,
                TetFile& file)
{
    const std::string key = Key(texture);
    std::string error;
    if (const auto it = state.tetBytes.find(key); it != state.tetBytes.end()) {
        bytes = it->second;
    } else if (!engine.TetBytes(texture, bytes, error)) {
        state.message = error;
        return false;
    }
    if (!TetFile::Parse(bytes, file, error)) {
        state.message = x2::ui_editor::TetName(texture) + ": " + error;
        return false;
    }
    state.tetSaved.try_emplace(key, bytes);
    return true;
}

// Moves what the script calls set by (dx, dy) layout units, in the scripts' text (x2::ui_editor::MoveCall), and builds
// again the dialogs made from them. One step of undo; merge: an id that joins the steps of one drag (0: none).
bool MoveSources(x2::ui_editor::Engine& engine, EditorState& state, const std::vector<SourceRef>& refs, float dx,
                 float dy, unsigned merge)
{
    if (dx == 0.0f && dy == 0.0f) {
        return false;
    }
    // Each call once (a control's pictures may share a line), by script.
    std::map<std::string, std::pair<std::string, std::set<std::pair<int, int>>>> calls;
    for (const SourceRef& ref : refs) {
        if (ref.Known()) {
            auto& entry = calls[Key(ref.script)];
            entry.first = ref.script;
            entry.second.insert({ref.line, static_cast<int>(ref.kind)});
        }
    }
    if (calls.empty()) {
        state.message = "not placed by a script call the editor follows";
        return false;
    }
    std::vector<EditorState::Step::Text> texts;
    for (const auto& [key, entry] : calls) {
        std::string text, error;
        if (!CurrentScript(engine, state, entry.first, text)) {
            return false;
        }
        const std::string before = text;
        for (const auto& [line, kind] : entry.second) {
            if (!x2::ui_editor::MoveCall(text, line, static_cast<x2::ui_editor::CallKind>(kind), dx, dy, error)) {
                state.message = entry.first + ": " + error;
                return false;
            }
        }
        texts.push_back({entry.first, before, text});
    }
    ApplyTexts(engine, state, texts, merge);
    return true;
}

// The texts of an edit (scripts, .tet files) made so, as one step of undo; merge: an id that joins the steps of one
// drag (0: none).
void ApplyTexts(x2::ui_editor::Engine& engine, EditorState& state, const std::vector<EditorState::Step::Text>& texts,
                unsigned merge)
{
    const auto sameScripts = [&](const EditorState::Step& step) {
        if (step.anchors || step.texts.size() != texts.size()) {
            return false;
        }
        for (std::size_t i = 0; i < texts.size(); ++i) {
            if (Key(step.texts[i].script) != Key(texts[i].script) || step.texts[i].tet != texts[i].tet) {
                return false;
            }
        }
        return true;
    };
    if (merge != 0 && !state.undo.empty() && state.undo.back().merge == merge && sameScripts(state.undo.back())) {
        for (std::size_t i = 0; i < texts.size(); ++i) {
            state.undo.back().texts[i].after = texts[i].after;
        }
    } else {
        EditorState::Step step;
        step.anchors = false;
        step.script = texts.front().script;
        step.texts = texts;
        step.merge = merge;
        state.undo.push_back(std::move(step));
    }
    state.redo.clear();
    for (const EditorState::Step::Text& text : texts) {
        if (text.tet) {
            SetTet(engine, state, text.script, text.after);
        } else {
            SetScript(engine, state, text.script, text.after);
        }
    }
}

// Writes the scripts whose text changed to their loose files in the client_data.
bool SaveScripts(EditorState& state)
{
    for (const auto& [key, text] : state.scriptTexts) {
        if (const auto saved = state.scriptSaved.find(key); saved != state.scriptSaved.end() && saved->second == text) {
            continue;
        }
        const std::string& name = state.scriptNames[key];
        const std::filesystem::path path = x2::ui_editor::FindLooseScript(state.gameFolder, name);
        if (path.empty()) {
            state.message = name + ": not in the client_data (data/)";
            return false;
        }
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        out.write(text.data(), static_cast<std::streamsize>(text.size()));
        if (!out) {
            state.message = "cannot write " + path.string();
            return false;
        }
        state.scriptSaved[key] = text;
        state.message = "saved " + path.string();
    }
    return true;
}

// Writes the .tet files whose bytes changed to their loose files in the client_data.
bool SaveTets(EditorState& state)
{
    for (const auto& [key, bytes] : state.tetBytes) {
        if (const auto saved = state.tetSaved.find(key); saved != state.tetSaved.end() && saved->second == bytes) {
            continue;
        }
        const std::string name = x2::ui_editor::TetName(state.tetTextures[key]);
        const std::filesystem::path path = x2::ui_editor::FindLooseScript(state.gameFolder, name);
        if (path.empty()) {
            state.message = name + ": not in the client_data (data/)";
            return false;
        }
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
        if (!out) {
            state.message = "cannot write " + path.string();
            return false;
        }
        state.tetSaved[key] = bytes;
        state.message = "saved " + path.string();
    }
    return true;
}

void ReapplyAnchors(x2::ui_editor::Engine& engine, const EditorState& state)
{
    for (int i = 0; i < static_cast<int>(engine.DialogCount()); ++i) {
        const std::string& script = engine.DialogScript(i);
        if (state.anchorsEdited.count(Key(script)) != 0) {
            engine.ApplyAnchors(i, state.anchors.Get(script), {});
        }
    }
}

bool SaveAnchors(EditorState& state)
{
    std::string error;
    if (!state.anchors.Save(state.anchorsPath, error)) {
        state.message = error;
        return false;
    }
    state.message = "saved " + state.anchorsPath.string();
    return true;
}

// Saves what changed: UIAnchors.lua, the scripts and the .tet files.
bool SaveAll(EditorState& state)
{
    return (!state.anchors.Changed() || SaveAnchors(state)) && SaveScripts(state) && SaveTets(state);
}

// The nearest of the editor's aspects and heights to a resolution.
void ChooseSize(EditorState& state, unsigned w, unsigned h)
{
    if (w == 0 || h == 0) {
        return;
    }
    const float ratio = static_cast<float>(w) / static_cast<float>(h);
    for (int a = 0; a < static_cast<int>(std::size(kAspects)); ++a) {
        if (std::fabs(kAspects[a].ratio - ratio) < std::fabs(kAspects[state.aspect].ratio - ratio)) {
            state.aspect = a;
        }
    }
    for (int k = 0; k < static_cast<int>(std::size(kHeights)); ++k) {
        const auto distance = [h](unsigned v) { return v > h ? v - h : h - v; };
        if (distance(kHeights[k]) < distance(kHeights[state.height])) {
            state.height = k;
        }
    }
}

void LoadRecord(EditorState& state)
{
    const std::filesystem::path file = state.gameFolder / "UIAnchorsRecord.tsv";
    std::error_code ec;
    state.recordTime = std::filesystem::last_write_time(file, ec);
    state.record = x2::ui_editor::Record::Load(file);
    state.scenes = state.record.Scenes();
    if (state.scene > static_cast<int>(state.scenes.size())) {
        state.scene = 0;
    }
    const std::vector<std::string> in =
        state.scene > 0 ? state.record.ScriptsIn(state.scenes[static_cast<std::size_t>(state.scene - 1)])
                        : std::vector<std::string>{};
    state.sceneScripts = {in.begin(), in.end()};
}

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

// Makes the open dialogs those the game shows, in its order: the ones of the same script kept (the n-th open of a
// script stands for the game's n-th), the others closed, the missing ones opened; then each takes the game's state.
void Mirror(x2::ui_editor::Engine& engine, EditorState& state)
{
    const auto& wanted = state.live.dialogs;
    std::vector<std::string> keys;     // as LiveSnapshot::Dialog::Key
    for (int i = 0; i < static_cast<int>(engine.DialogCount()); ++i) {
        std::string key = Key(engine.DialogScript(i));
        for (const std::string& extra : engine.DialogExtraScripts(i)) {
            key += '+' + Key(extra);
        }
        keys.push_back(std::move(key));
    }
    std::vector<int> match(wanted.size(), -1);
    std::vector<bool> used(keys.size(), false);
    for (std::size_t w = 0; w < wanted.size(); ++w) {
        for (std::size_t i = 0; i < keys.size(); ++i) {
            if (!used[i] && keys[i] == wanted[w].Key()) {
                match[w] = static_cast<int>(i);
                used[i] = true;
                break;
            }
        }
    }
    bool closed = false;
    for (int i = static_cast<int>(keys.size()) - 1; i >= 0; --i) {
        if (used[static_cast<std::size_t>(i)]) {
            continue;
        }
        engine.CloseDialog(i);
        closed = true;
        for (int& m : match) {
            if (m > i) {
                --m;
            }
        }
    }
    std::string failed;
    for (std::size_t w = 0; w < wanted.size(); ++w) {
        if (match[w] >= 0) {
            continue;
        }
        const auto listed = [&](const std::string& key) {
            const auto it = state.scriptByKey.find(key);
            return it != state.scriptByKey.end() ? it->second : key;
        };
        std::string error;
        match[w] = engine.OpenDialog(listed(wanted[w].script), error);
        if (match[w] < 0) {
            failed = error;
            continue;
        }
        for (const std::string& extra : wanted[w].extraScripts) {
            if (!engine.LoadScriptInto(match[w], listed(extra), error)) {
                failed = error;
            }
        }
    }
    for (std::size_t w = 0; w < wanted.size(); ++w) {
        engine.ApplyLive(match[w], wanted[w]);
    }
    if (closed) {
        state.selected = {};
    }
    ChooseSize(state, state.live.screen[0], state.live.screen[1]);
    state.exact[0] = state.live.screen[0];
    state.exact[1] = state.live.screen[1];
    state.message = failed.empty() ? "game: " + (state.live.scene.empty() ? std::string("starting") : state.live.scene)
                                   : failed;
}

// Watch game, a few times a second: the snapshot when it changed, and the record the game keeps writing.
void Poll(x2::ui_editor::Engine& engine, EditorState& state)
{
    if (state.game != nullptr && SDL_WaitProcess(state.game, false, nullptr)) {
        SDL_DestroyProcess(state.game);
        state.game = nullptr;
    }
    const std::uint64_t now = SDL_GetTicks();
    if (!state.watch || now - state.lastPoll < 250) {
        return;
    }
    state.lastPoll = now;
    std::error_code ec;
    const auto recordTime = std::filesystem::last_write_time(state.gameFolder / "UIAnchorsRecord.tsv", ec);
    if (!ec && recordTime != state.recordTime) {
        LoadRecord(state);
    }
    const std::filesystem::path file = state.gameFolder / "UILive.tsv";
    const auto liveTime = std::filesystem::last_write_time(file, ec);
    if (ec || liveTime == state.liveTime) {
        return;
    }
    state.liveTime = liveTime;
    state.live = x2::ui_editor::LiveSnapshot::Load(file);
    Mirror(engine, state);
}

// The client in the game folder, with the live snapshot and the anchor recorder on; its output goes to the editor's.
void StartGame(EditorState& state)
{
    // This platform's client only: a game folder can hold both (the Windows build is run there under Wine too).
#ifdef _WIN32
    const std::filesystem::path program = state.gameFolder / "x2.exe";
#else
    const std::filesystem::path program = state.gameFolder / "x2";
#endif
    if (!std::filesystem::exists(program)) {
        state.message = "no x2 in " + state.gameFolder.string() + " (tools/client/install.sh)";
        return;
    }
    const std::string programName = program.string(), folder = state.gameFolder.string();
    SDL_Environment* environment = SDL_CreateEnvironment(true);
    SDL_SetEnvironmentVariable(environment, "X2_UI_LIVE", "1", true);
    SDL_SetEnvironmentVariable(environment, "X2_RECORD_UI_ANCHORS", "1", true);
    const char* args[] = {programName.c_str(), nullptr};
    const SDL_PropertiesID properties = SDL_CreateProperties();
    SDL_SetPointerProperty(properties, SDL_PROP_PROCESS_CREATE_ARGS_POINTER, static_cast<void*>(args));
    SDL_SetPointerProperty(properties, SDL_PROP_PROCESS_CREATE_ENVIRONMENT_POINTER, environment);
    SDL_SetStringProperty(properties, SDL_PROP_PROCESS_CREATE_WORKING_DIRECTORY_STRING, folder.c_str());
    SDL_SetNumberProperty(properties, SDL_PROP_PROCESS_CREATE_STDIN_NUMBER, SDL_PROCESS_STDIO_NULL);
    SDL_SetNumberProperty(properties, SDL_PROP_PROCESS_CREATE_STDOUT_NUMBER, SDL_PROCESS_STDIO_INHERITED);
    SDL_SetNumberProperty(properties, SDL_PROP_PROCESS_CREATE_STDERR_NUMBER, SDL_PROCESS_STDIO_INHERITED);
    state.game = SDL_CreateProcessWithProperties(properties);
    SDL_DestroyProperties(properties);
    SDL_DestroyEnvironment(environment);
    if (state.game == nullptr) {
        state.message = std::string("starting the game: ") + SDL_GetError();
        return;
    }
    state.watch = true;
    state.message = "game started";
}

void WatchControls(EditorState& state)
{
    if (ImGui::Checkbox("Watch game", &state.watch) && state.watch) {
        state.liveTime = {};    // mirror the current snapshot at once
        state.lastPoll = 0;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Follow the running game; unchecked, the screen stays as it was last");
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(state.game != nullptr);
    if (ImGui::SmallButton("Start game")) {
        StartGame(state);
    }
    ImGui::EndDisabled();
    if (state.watch) {
        if (state.liveTime == std::filesystem::file_time_type{}) {
            ImGui::TextDisabled("waiting for UILive.tsv (a client run with X2_UI_LIVE=1)");
        } else {
            ImGui::TextDisabled("%s, %zu dialogs", state.live.scene.empty() ? "(no state yet)" : state.live.scene.c_str(),
                                state.live.dialogs.size());
        }
    }
}

void SaveControls(x2::ui_editor::Engine& engine, EditorState& state)
{
    const bool typing = ImGui::GetIO().WantTextInput;
    ImGui::BeginDisabled(state.undo.empty());
    if (ImGui::SmallButton("Undo") || (!typing && ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Z))) {
        Undo(engine, state, false);
    }
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Ctrl+Z: %s", state.undo.empty() ? "nothing to undo" : state.undo.back().script.c_str());
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(state.redo.empty());
    if (ImGui::SmallButton("Redo") || (!typing && (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Y) ||
                                                   ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Z)))) {
        Undo(engine, state, true);
    }
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Ctrl+Y or Ctrl+Shift+Z: %s",
                          state.redo.empty() ? "nothing to redo" : state.redo.back().script.c_str());
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(!state.Changed());
    if (ImGui::SmallButton("Save") || (!typing && state.Changed() && ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_S))) {
        SaveAll(state);
    }
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        std::string files;
        if (state.anchors.Changed()) {
            files += "\n" + state.anchorsPath.string();
        }
        for (const auto& [key, text] : state.scriptTexts) {
            if (const auto saved = state.scriptSaved.find(key); saved == state.scriptSaved.end() || saved->second != text) {
                files += "\n" + state.scriptNames[key];
            }
        }
        for (const auto& [key, bytes] : state.tetBytes) {
            if (const auto saved = state.tetSaved.find(key); saved == state.tetSaved.end() || saved->second != bytes) {
                files += "\n" + x2::ui_editor::TetName(state.tetTextures[key]);
            }
        }
        ImGui::SetTooltip("Ctrl+S: UIAnchors.lua, the dialog scripts and the .tet files edited%s\nThe game reads them when it starts.",
                          files.c_str());
    }
    ImGui::SameLine();
    ImGui::TextDisabled("%s", state.Changed() ? "unsaved changes" : "no changes");
    if (!state.anchors.Problem().empty()) {
        ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "UIAnchors.lua, %s", state.anchors.Problem().c_str());
    }
}

void DialogsPanel(x2::ui_editor::Engine& engine, EditorState& state)
{
    WatchControls(state);
    SaveControls(engine, state);
    ImGui::Separator();
    ImGui::SetNextItemWidth(-FLT_MIN - ImGui::CalcTextSize("Open scene").x - ImGui::GetStyle().FramePadding.x * 4);
    const char* preview = state.scene == 0 ? "All scripts" : state.scenes[static_cast<std::size_t>(state.scene - 1)].c_str();
    if (ImGui::BeginCombo("##scene", preview)) {
        for (int i = 0; i <= static_cast<int>(state.scenes.size()); ++i) {
            const char* name = i == 0 ? "All scripts" : state.scenes[static_cast<std::size_t>(i - 1)].c_str();
            if (ImGui::Selectable(name, state.scene == i)) {
                state.scene = i;
                LoadRecord(state);
            }
        }
        ImGui::EndCombo();
    }
    if (ImGui::IsItemHovered() && state.scenes.empty()) {
        ImGui::SetTooltip("No scenes recorded: run the game with X2_RECORD_UI_ANCHORS=1 (\"Start game\" does)");
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(state.scene == 0);
    if (ImGui::Button("Open scene")) {
        state.watch = false;
        engine.CloseAll();
        for (const std::string& script : state.scripts) {
            if (state.sceneScripts.count(Key(script)) != 0) {
                OpenScript(engine, state, script);
            }
        }
        state.selected = {};
    }
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Every script the recording saw in this game state, at its first recorded position");
    }
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::InputTextWithHint("##filter", "filter dialogs", state.filter, sizeof(state.filter));
    ImGui::TextDisabled("double-click opens, Ctrl adds to the open ones");
    if (ImGui::BeginChild("scripts", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.6f), ImGuiChildFlags_Borders)) {
        for (const std::string& script : state.scripts) {
            if (!ContainsNoCase(script, state.filter)
                || (state.scene > 0 && state.sceneScripts.count(Key(script)) == 0)) {
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

using MoveFunction = std::function<void(const std::vector<SourceRef>&, float, float, unsigned)>;

// Where a script set something, for the inspector.
std::string SourceLabel(const SourceRef& ref)
{
    if (!ref.Known()) {
        return "not set by a script call";
    }
    std::string label = ref.script + ":" + std::to_string(ref.line);
    if (ref.shared > 1) {
        label += " (sets " + std::to_string(ref.shared) + ")";
    }
    return label;
}

// What moving a control moves: the point tables set on it, its pictures' and its texts'.
std::vector<SourceRef> ControlSources(const ControlInfo& control)
{
    std::vector<SourceRef> refs = control.points;
    for (const auto& picture : control.pictures) {
        refs.push_back(picture.point);
    }
    for (const auto& string : control.strings) {
        refs.push_back(string.source);
    }
    refs.erase(std::remove_if(refs.begin(), refs.end(), [](const SourceRef& ref) { return !ref.Known(); }), refs.end());
    return refs;
}

// A position the inspector moves, dragged or typed in whole layout units, then the lines that set it. Calls move
// with the change; one drag is one undo step.
void MoveField(EditorState& state, const char* label, const float value[2], const std::vector<SourceRef>& refs,
               const MoveFunction& move)
{
    float v[2] = {std::round(value[0]), std::round(value[1])};
    ImGui::BeginDisabled(refs.empty());
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
    const bool changed = ImGui::DragFloat2(label, v, 1.0f, -8192.0f, 8192.0f, "%.0f");
    ImGui::EndDisabled();
    const unsigned merge = ImGui::GetItemID();
    if (ImGui::IsItemDeactivated() && !state.undo.empty()) {
        state.undo.back().merge = 0;    // the drag ended: the next one is a step of its own
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        std::string lines;
        for (const SourceRef& ref : refs) {
            lines += "\n" + SourceLabel(ref);
        }
        ImGui::SetTooltip("%s", refs.empty() ? "Not set by a script call the editor follows (code, or no script)"
                                             : ("Moves what these script lines set:" + lines).c_str());
    }
    if (changed) {
        const float dx = std::round(v[0]) - std::round(value[0]), dy = std::round(v[1]) - std::round(value[1]);
        if (dx != 0.0f || dy != 0.0f) {
            move(refs, dx, dy, merge);
        }
    }
    const auto shared = std::find_if(refs.begin(), refs.end(), [](const SourceRef& ref) { return ref.shared > 1; });
    if (shared != refs.end()) {
        ImGui::TextColored(ImVec4(1, 0.75f, 0.3f, 1), "%s: a line that sets %d things moves them all",
                           SourceLabel(*shared).c_str(), shared->shared);
    }
}

std::string LowerText(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return text;
}

// A texture's .tet, kept as read so the inspector does not load it every frame. False when it has none.
bool PictureTet(x2::ui_editor::Engine& engine, EditorState& state, const std::string& texture, TetFile& file)
{
    const std::string key = Key(texture);
    if (texture.empty() || state.noTet.count(key) != 0) {
        return false;
    }
    std::string bytes, error;
    if (const auto it = state.tetBytes.find(key); it != state.tetBytes.end()) {
        bytes = it->second;
    } else if (!engine.TetBytes(texture, bytes, error)) {
        state.noTet.insert(key);
        return false;
    }
    if (!TetFile::Parse(bytes, file, error)) {
        state.noTet.insert(key);
        return false;
    }
    state.tetBytes.try_emplace(key, bytes);
    state.tetSaved.try_emplace(key, bytes);
    state.tetTextures.try_emplace(key, texture);
    return true;
}

// The script lines that name a texture's key, searched once a run (the files as saved).
const std::vector<x2::ui_editor::KeyUse>& KeyUses(EditorState& state, const std::string& texture, const std::string& key)
{
    const std::string id = LowerText(texture + "|" + key);
    auto it = state.keyUses.find(id);
    if (it == state.keyUses.end()) {
        it = state.keyUses.emplace(id, x2::ui_editor::FindKeyUses(state.gameFolder, texture, key)).first;
    }
    return it->second;
}

// Whether the UV rectangle of a picture's key may change now: yes when only its own SetTex names it, or the user chose
// to change it in place; otherwise asks (KeyPopup) and says no.
bool MayChangeKey(EditorState& state, const DialogInfo& dialog, const PictureInfo& picture)
{
    const std::string id = LowerText(picture.texture + "|" + picture.key);
    if (state.keysInPlace.count(id) != 0) {
        return true;
    }
    const auto& uses = KeyUses(state, picture.texture, picture.key);
    const std::string own = Key(picture.texSource.script);
    const bool others = picture.texSource.shared > 1 || std::any_of(uses.begin(), uses.end(), [&](const auto& use) {
        return Key(use.script.filename().string()) != own || use.line != picture.texSource.line;
    });
    if (!others && picture.texSource.Known()) {
        state.keysInPlace.insert(id);
        return true;
    }
    if (!state.keyAsk.open) {
        auto& ask = state.keyAsk;
        ask = {};
        ask.open = true;
        state.message.clear();
        ask.texture = picture.texture;
        ask.key = picture.key;
        ask.dialog = dialog.script;
        ask.texSource = picture.texSource;
        ask.uses = uses;
        // The key named after the dialog: BG_LEFT_TOP of DLG_UI_Local_Map_NEW.lua -> BG_LEFT_TOP_UI_LOCAL_MAP_NEW.
        std::string stem = std::filesystem::path(dialog.script).stem().string();
        if (LowerText(stem.substr(0, 4)) == "dlg_") {
            stem = stem.substr(4);
        }
        std::string name = x2::ui_editor::UpperKey(picture.key + "_" + stem);
        name.erase(std::remove_if(name.begin(), name.end(), [](unsigned char c) { return std::isalnum(c) == 0 && c != '_'; }),
                   name.end());
        std::snprintf(ask.name, sizeof(ask.name), "%s", name.c_str());
    }
    return false;
}

// A picture's SetTex made to name this texture and key, as one step of undo.
void ChangeTexture(x2::ui_editor::Engine& engine, EditorState& state, const PictureInfo& picture,
                   const std::string& texture, const std::string& key)
{
    std::string text, error;
    if (!picture.texSource.Known() || !CurrentScript(engine, state, picture.texSource.script, text)) {
        return;
    }
    const std::string before = text;
    if (!x2::ui_editor::SetTexCall(text, picture.texSource.line, texture, key, error)) {
        state.message = picture.texSource.script + ": " + error;
        return;
    }
    state.keyUses.clear();
    ApplyTexts(engine, state, {{picture.texSource.script, before, text}}, 0);
}

// Moves (dx, dy) or grows (dw, dh) the UV rectangle of a picture's key, in texels, in its texture's .tet.
void ChangeUv(x2::ui_editor::Engine& engine, EditorState& state, const PictureInfo& picture, float dx, float dy, float dw,
              float dh, unsigned merge)
{
    std::string bytes;
    TetFile file;
    if (!CurrentTet(engine, state, picture.texture, bytes, file)) {
        return;
    }
    const TetRect* found = file.Find(picture.key);
    if (found == nullptr) {
        state.message = picture.texture + " has no key " + picture.key;
        return;
    }
    TetRect rect = *found;
    rect.Move(dx, dy);
    if ((dw != 0.0f || dh != 0.0f) && !rect.Resize(dw, dh)) {
        state.message = picture.key + " is rotated in its .tet: only moved";
        return;
    }
    file.Set(picture.key, rect);
    ApplyTexts(engine, state, {{picture.texture, bytes, file.Bytes(), true}}, merge);
}

// A shared key's UV about to change: change it in place, or give this picture a key of its own (a copy of the
// rectangle under a new name, which its SetTex then names), as one step of undo.
void KeyPopup(x2::ui_editor::Engine& engine, EditorState& state)
{
    auto& ask = state.keyAsk;
    if (ask.open) {
        ImGui::OpenPopup("Shared key");
    }
    if (!ImGui::BeginPopupModal("Shared key", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        return;
    }
    ImGui::Text("%s of %s is named by %zu script line(s):", ask.key.c_str(), ask.texture.c_str(), ask.uses.size());
    if (ImGui::BeginChild("uses", ImVec2(560, std::min(240.0f, 20.0f + 18.0f * static_cast<float>(ask.uses.size()))),
                          ImGuiChildFlags_Borders)) {
        for (const auto& use : ask.uses) {
            ImGui::Text("%s:%d", use.script.generic_string().c_str(), use.line);
        }
    }
    ImGui::EndChild();
    if (ask.texSource.shared > 1) {
        ImGui::TextColored(ImVec4(1, 0.75f, 0.3f, 1), "Its own SetTex line sets %d pictures.", ask.texSource.shared);
    }
    ImGui::TextDisabled("The game's code may name it too: it is not searched.");
    const auto close = [&] {
        ask.open = false;
        ImGui::CloseCurrentPopup();
    };
    if (ImGui::Button("Change it in place")) {
        state.keysInPlace.insert(LowerText(ask.texture + "|" + ask.key));
        state.message = ask.key + ": UV changes go to every picture that names it";
        close();
    }
    ImGui::Separator();
    ImGui::BeginDisabled(!ask.texSource.Known());
    ImGui::SetNextItemWidth(360);
    ImGui::InputText("##name", ask.name, sizeof(ask.name), ImGuiInputTextFlags_CharsUppercase);
    ImGui::SameLine();
    if (ImGui::Button("Make a new key")) {
        std::string bytes, error, text;
        TetFile file;
        const TetRect* rect = nullptr;
        if (CurrentTet(engine, state, ask.texture, bytes, file) && (rect = file.Find(ask.key)) != nullptr &&
            CurrentScript(engine, state, ask.texSource.script, text)) {
            const std::string before = text;
            const std::string name = ask.name;
            if (!file.Add(name, *rect, error) ||
                !x2::ui_editor::SetTexCall(text, ask.texSource.line, ask.texture, x2::ui_editor::UpperKey(name), error)) {
                state.message = error;
            } else {
                ApplyTexts(engine, state,
                           {{ask.texSource.script, before, text}, {ask.texture, bytes, file.Bytes(), true}}, 0);
                state.keyUses.clear();
                state.keysInPlace.insert(LowerText(ask.texture + "|" + name));
                state.message = x2::ui_editor::UpperKey(name) + ": a copy of " + ask.key + ", now named by " +
                                ask.texSource.script + ":" + std::to_string(ask.texSource.line);
                close();
            }
        }
    }
    ImGui::EndDisabled();
    if (!ask.texSource.Known()) {
        ImGui::TextDisabled("No SetTex line the editor follows names it here: only in place.");
    }
    if (!state.message.empty() && ImGui::IsPopupOpen("Shared key")) {
        ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "%s", state.message.c_str());
    }
    if (ImGui::Button("Cancel")) {
        close();
    }
    ImGui::EndPopup();
}

// addZone: makes a picture's width a stretch zone of its dialog.
void PictureTable(x2::ui_editor::Engine& engine, EditorState& state, const DialogInfo& dialog, const ControlInfo& control,
                  const std::function<void(float, float)>& addZone, const MoveFunction& move)
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
            ImGui::TextDisabled("SetPoint: %s", SourceLabel(picture.point).c_str());
            MoveField(state, "left top", picture.points[0],
                      picture.point.Known() ? std::vector<SourceRef>{picture.point} : std::vector<SourceRef>{}, move);
            float lo = picture.points[0][0], hi = lo;
            for (const auto& point : picture.points) {
                lo = std::min(lo, point[0]);
                hi = std::max(hi, point[0]);
            }
            if (hi > lo && ImGui::SmallButton("Make its width a stretch zone")) {
                addZone(lo + control.offset[0], hi + control.offset[0]);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("The dialog's STRETCH controls grow there: a plain middle between end pieces");
            }

            // Texture and key: the SetTex call's names.
            ImGui::TextDisabled("SetTex: %s", SourceLabel(picture.texSource).c_str());
            TetFile tet;
            const bool hasTet = PictureTet(engine, state, picture.texture, tet);
            ImGui::BeginDisabled(!picture.texSource.Known());
            char texture[128];
            std::snprintf(texture, sizeof(texture), "%s", picture.texture.c_str());
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
            if (ImGui::InputText("texture", texture, sizeof(texture), ImGuiInputTextFlags_EnterReturnsTrue) &&
                picture.texture != texture) {
                // The same key when the new texture has one, else its first.
                TetFile other;
                std::string key = picture.key;
                if (PictureTet(engine, state, texture, other) && other.Find(key) == nullptr && !other.Entries().empty()) {
                    key = other.Entries().front().key;
                }
                ChangeTexture(engine, state, picture, texture, key);
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("A texture file the game loads (client_data or archives); Enter changes its SetTex");
            }
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
            if (ImGui::BeginCombo("key", picture.key.c_str(), ImGuiComboFlags_HeightLarge)) {
                for (const auto& entry : tet.Entries()) {
                    if (ImGui::Selectable(entry.key.c_str(), LowerText(entry.key) == LowerText(picture.key))) {
                        ChangeTexture(engine, state, picture, picture.texture, entry.key);
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("%.0f, %.0f  %.0f x %.0f", static_cast<double>(entry.rect.leftTop[0]),
                                          static_cast<double>(entry.rect.leftTop[1]),
                                          static_cast<double>(entry.rect.size[0]), static_cast<double>(entry.rect.size[1]));
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::EndDisabled();

            // The key's UV rectangle in the texture's .tet.
            const TetRect* rect = hasTet ? tet.Find(picture.key) : nullptr;
            if (rect != nullptr) {
                const auto uvField = [&](const char* name, const float value[2], bool size, bool enabled) {
                    float v[2] = {value[0], value[1]};
                    ImGui::BeginDisabled(!enabled);
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
                    const bool changed = ImGui::DragFloat2(name, v, 1.0f, -16384.0f, 16384.0f, "%.0f");
                    ImGui::EndDisabled();
                    const unsigned merge = ImGui::GetItemID();
                    if (ImGui::IsItemDeactivated() && !state.undo.empty()) {
                        state.undo.back().merge = 0;
                    }
                    const float dx = std::round(v[0] - value[0]), dy = std::round(v[1] - value[1]);
                    if (changed && (dx != 0.0f || dy != 0.0f) && MayChangeKey(state, dialog, picture)) {
                        ChangeUv(engine, state, picture, size ? 0.0f : dx, size ? 0.0f : dy, size ? dx : 0.0f,
                                 size ? dy : 0.0f, merge);
                    }
                };
                uvField("UV left top", rect->leftTop, false, true);
                uvField("UV size", rect->size, true, rect->Upright());
                if (!rect->Upright()) {
                    ImGui::TextDisabled("rotated in its .tet: it moves, its size stays");
                }
            } else if (!picture.texture.empty()) {
                ImGui::TextDisabled(hasTet ? "the key is not in the texture's .tet" : "no .tet: the whole texture");
            }

            static const char* const corners[] = {"left top", "right top", "left bottom", "right bottom"};
            if (ImGui::BeginTable("corners", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit)) {
                ImGui::TableSetupColumn("corner");
                ImGui::TableSetupColumn("point");
                ImGui::TableSetupColumn("UV (texels)");
                ImGui::TableHeadersRow();
                const float* uv[4] = {};
                if (rect != nullptr) {
                    uv[0] = rect->leftTop, uv[1] = rect->rightTop, uv[2] = rect->leftBottom, uv[3] = rect->rightBottom;
                }
                for (int k = 0; k < 4; ++k) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(corners[k]);
                    ImGui::TableNextColumn();
                    ImGui::Text("%.1f, %.1f", static_cast<double>(picture.points[k][0]),
                                static_cast<double>(picture.points[k][1]));
                    ImGui::TableNextColumn();
                    if (uv[k] != nullptr) {
                        ImGui::Text("%.0f, %.0f", static_cast<double>(uv[k][0]), static_cast<double>(uv[k][1]));
                    } else {
                        ImGui::TextDisabled("-");
                    }
                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}

void StringTable(EditorState& state, const ControlInfo& control, const MoveFunction& move)
{
    if (control.strings.empty()) {
        return;
    }
    ImGui::SeparatorText("Texts");
    for (std::size_t t = 0; t < control.strings.size(); ++t) {
        const auto& string = control.strings[t];
        ImGui::PushID(static_cast<int>(t));
        const std::string label = std::to_string(t + 1) + ": " +
                                  (string.text.empty() ? std::string("(empty: filled by the game's code)") : string.text);
        if (ImGui::TreeNode(label.c_str())) {
            if (string.stringId >= 0) {
                ImGui::Text("string table: STR_ID_%d", string.stringId);
            } else {
                ImGui::TextDisabled("no string table entry");
            }
            ImGui::TextDisabled("AddString: %s", SourceLabel(string.source).c_str());
            MoveField(state, "position", string.pos,
                      string.source.Known() ? std::vector<SourceRef>{string.source} : std::vector<SourceRef>{}, move);
            ImGui::Text("font: %d, alignment flags: 0x%x", string.font, string.sortFlag);
            ImGui::ColorButton("##color", ImVec4(string.color[0], string.color[1], string.color[2], string.color[3]));
            ImGui::SameLine();
            ImGui::Text("color %.2f, %.2f, %.2f, %.2f", static_cast<double>(string.color[0]),
                        static_cast<double>(string.color[1]), static_cast<double>(string.color[2]),
                        static_cast<double>(string.color[3]));
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}

constexpr const char* kAnchorChoices[] = {"CENTER", "LEFT", "RIGHT", "STRETCH"};

// A choice of `none` or one of the first `count` anchors; true when one was picked.
bool AnchorCombo(const char* label, const char* none, std::string& value, int count)
{
    bool picked = false;
    if (ImGui::BeginCombo(label, value.empty() ? none : value.c_str())) {
        if (ImGui::Selectable(none, value.empty())) {
            value.clear();
            picked = true;
        }
        for (int i = 0; i < count; ++i) {
            if (ImGui::Selectable(kAnchorChoices[i], value == kAnchorChoices[i])) {
                value = kAnchorChoices[i];
                picked = true;
            }
        }
        ImGui::EndCombo();
    }
    return picked;
}

const char* AnchorSource(bool declared, bool fromScript, const char* none)
{
    return !declared ? none : fromScript ? " (the script's SetAnchor)" : "";
}

void DialogAnchorEditor(x2::ui_editor::Engine& engine, EditorState& state, const DialogInfo& dialog)
{
    AnchorEntry entry = state.anchors.Get(dialog.script);
    ImGui::BeginDisabled(dialog.anchorFromScript);
    if (AnchorCombo("UIAnchors.lua##dialog", "(none: centered)", entry.dialog, 3)) {
        EditAnchors(engine, state, dialog.script, entry);
    }
    ImGui::EndDisabled();
    if (dialog.anchorFromScript && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("The script sets it (SetAnchor), which wins over UIAnchors.lua");
    }
    if (ImGui::TreeNode("zones", "stretch zones: %zu", entry.zones.size())) {
        ImGui::TextDisabled("where its STRETCH controls grow (layout x from the dialog); none: scaled whole");
        for (std::size_t z = 0; z < entry.zones.size(); ++z) {
            ImGui::PushID(static_cast<int>(z));
            float zone[2] = {entry.zones[z].first, entry.zones[z].second};
            ImGui::SetNextItemWidth(-FLT_MIN - ImGui::CalcTextSize("x").x - ImGui::GetStyle().FramePadding.x * 4);
            if (ImGui::DragFloat2("##zone", zone, 1.0f, -4096.0f, 4096.0f, "%.0f") && zone[0] < zone[1]) {
                entry.zones[z] = {zone[0], zone[1]};
                EditAnchors(engine, state, dialog.script, entry, ImGui::GetItemID());
            }
            if (ImGui::IsItemDeactivated() && !state.undo.empty()) {
                state.undo.back().merge = 0;    // the drag ended: the next one is a step of its own
            }
            ImGui::SameLine();
            const bool remove = ImGui::SmallButton("x");
            ImGui::PopID();
            if (remove) {
                entry.zones.erase(entry.zones.begin() + static_cast<std::ptrdiff_t>(z));
                EditAnchors(engine, state, dialog.script, entry);
                break;
            }
        }
        if (ImGui::SmallButton("Add zone")) {
            entry.zones.emplace_back(0.0f, 1024.0f);
            EditAnchors(engine, state, dialog.script, entry);
        }
        ImGui::TreePop();
    }
}

void ControlAnchorEditor(x2::ui_editor::Engine& engine, EditorState& state, const DialogInfo& dialog,
                         const ControlInfo& control)
{
    AnchorEntry entry = state.anchors.Get(dialog.script);
    std::string value = control.named ? entry.Control(control.name) : std::string();
    ImGui::BeginDisabled(!control.named || control.anchorFromScript);
    if (AnchorCombo("UIAnchors.lua##control", "(none: the dialog's)", value, 4)) {
        entry.SetControl(control.name, value);
        EditAnchors(engine, state, dialog.script, entry);
    }
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        if (!control.named) {
            ImGui::SetTooltip("UIAnchors.lua names its controls: this one has no name");
        } else if (control.anchorFromScript) {
            ImGui::SetTooltip("The script sets it (SetAnchor), which wins over UIAnchors.lua");
        }
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
    ImGui::Text("anchor: %s%s", dialog.anchor.c_str(),
                AnchorSource(dialog.anchorDeclared, dialog.anchorFromScript, " (default)"));
    DialogAnchorEditor(engine, state, dialog);
    const MoveFunction move = [&](const std::vector<SourceRef>& refs, float dx, float dy, unsigned merge) {
        MoveSources(engine, state, refs, dx, dy, merge);
    };
    ImGui::Text("position: %.0f, %.0f (drawn at %.0f, %.0f)", static_cast<double>(dialog.pos[0]),
                static_cast<double>(dialog.pos[1]), static_cast<double>(dialog.canvasPos[0]),
                static_cast<double>(dialog.canvasPos[1]));
    ImGui::TextDisabled("SetPos: %s", SourceLabel(dialog.posSource).c_str());
    MoveField(state, "move##dialog", dialog.pos,
              dialog.posSource.Known() ? std::vector<SourceRef>{dialog.posSource} : std::vector<SourceRef>{}, move);
    if (dialog.placedByCode) {
        ImGui::TextColored(ImVec4(1, 0.75f, 0.3f, 1), "The game's code places it: this moves its script's SetPos only");
    }
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
    ImGui::Text("anchor: %s%s", control.anchor.c_str(),
                AnchorSource(control.anchorDeclared, control.anchorFromScript, " (the dialog's)"));
    ControlAnchorEditor(engine, state, dialog, control);
    ImGui::Text("shown: %s", control.shown ? "yes" : "no");
    ImGui::Text("offset: %.0f, %.0f", static_cast<double>(control.offset[0]), static_cast<double>(control.offset[1]));
    const std::vector<SourceRef> sources = ControlSources(control);
    const float origin[2] = {control.drawn ? control.drawnMin[0] : control.offset[0],
                             control.drawn ? control.drawnMin[1] : control.offset[1]};
    MoveField(state, "move##control", origin, sources, move);
    ImGui::TextDisabled("its left top as drawn; on the screen drag it or use the arrows (Shift: 10), Alt for the dialog");
    if (control.drawn) {
        ImGui::Text("drawn: %.0f, %.0f to %.0f, %.0f (%.0f x %.0f)", static_cast<double>(control.drawnMin[0]),
                    static_cast<double>(control.drawnMin[1]), static_cast<double>(control.drawnMax[0]),
                    static_cast<double>(control.drawnMax[1]),
                    static_cast<double>(control.drawnMax[0] - control.drawnMin[0]),
                    static_cast<double>(control.drawnMax[1] - control.drawnMin[1]));
    } else {
        ImGui::TextDisabled("drew nothing in the last frame");
    }
    StringTable(state, control, move);
    PictureTable(engine, state, dialog, control, [&](float lo, float hi) {
        AnchorEntry entry = state.anchors.Get(dialog.script);
        entry.zones.emplace_back(lo, hi);
        std::sort(entry.zones.begin(), entry.zones.end());
        EditAnchors(engine, state, dialog.script, entry);
    }, move);
}

// Closing with unsaved changes (UIAnchors.lua, scripts) asks first; true when the editor should close.
bool QuitPopup(EditorState& state)
{
    if (state.quitAsked) {
        ImGui::OpenPopup("Unsaved changes");
        state.quitAsked = false;
    }
    bool quit = false;
    if (ImGui::BeginPopupModal("Unsaved changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("UIAnchors.lua, dialog scripts or .tet files have unsaved changes.");
        if (ImGui::Button("Save and close")) {
            quit = SaveAll(state);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Close without saving")) {
            quit = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    return quit;
}

// Moves the selected control (or, `dialog`, its dialog) in its scripts. False when nothing could be moved.
bool MoveSelected(x2::ui_editor::Engine& engine, EditorState& state, bool dialog, float dx, float dy, unsigned merge)
{
    const Selection& sel = state.selected;
    if (sel.dialog < 0 || sel.dialog >= static_cast<int>(state.inspected.size())) {
        return false;
    }
    const DialogInfo& info = state.inspected[static_cast<std::size_t>(sel.dialog)];
    if (dialog) {
        return MoveSources(engine, state, {info.posSource}, dx, dy, merge);
    }
    if (sel.control < 0 || sel.control >= static_cast<int>(info.controls.size())) {
        return false;
    }
    return MoveSources(engine, state, ControlSources(info.controls[static_cast<std::size_t>(sel.control)]), dx, dy, merge);
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

void ScreenPanel(x2::ui_editor::Engine& engine, const x2::app::ImGuiLayer& gui, EditorState& state)
{
    ImGui::SetNextItemWidth(90);
    if (ImGui::BeginCombo("aspect", kAspects[state.aspect].name)) {
        for (int i = 0; i < static_cast<int>(std::size(kAspects)); ++i) {
            if (ImGui::Selectable(kAspects[i].name, state.aspect == i && state.exact[0] == 0)) {
                state.aspect = i;
                state.exact[0] = state.exact[1] = 0;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    const std::string heightLabel = std::to_string(kHeights[state.height]);
    if (ImGui::BeginCombo("height", heightLabel.c_str())) {
        for (int i = 0; i < static_cast<int>(std::size(kHeights)); ++i) {
            if (ImGui::Selectable(std::to_string(kHeights[i]).c_str(), state.height == i && state.exact[0] == 0)) {
                state.height = i;
                state.exact[0] = state.exact[1] = 0;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::Text("%u x %u%s", engine.TargetWidth(), engine.TargetHeight(), state.exact[0] != 0 ? " (game)" : "");
    ImGui::SameLine();
    ImGui::Checkbox("fit", &state.fit);
    ImGui::SameLine();
    if (ImGui::SmallButton("100%")) {
        state.fit = false;
        state.zoom = 1.0f;
    }
    ImGui::SameLine();
    ImGui::TextDisabled("%.0f%%", static_cast<double>(state.zoom * 100.0f));
    ImGui::SameLine();
    ImGui::Checkbox("all boxes", &state.showBounds);
    if (!state.message.empty()) {
        ImGui::SameLine();
        ImGui::TextDisabled("%s", state.message.c_str());
    }

    const std::uint64_t texture = gui.TextureId(engine.Target());
    if (texture == 0) {
        return;
    }
    // The wheel zooms around the pointer, the right or middle button drags the view (the wheel does not scroll).
    if (ImGui::BeginChild("screen", ImVec2(0, 0), ImGuiChildFlags_None,
                          ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                              ImGuiWindowFlags_NoNavInputs)) {     // the arrows move the selection instead
        const float tw = static_cast<float>(engine.TargetWidth()), th = static_cast<float>(engine.TargetHeight());
        const ImVec2 avail = ImGui::GetContentRegionAvail();
        if (state.fit) {
            state.zoom = std::min(avail.x / tw, avail.y / th);
        }
        if (ImGui::IsWindowHovered()) {
            const ImGuiIO& io = ImGui::GetIO();
            if (io.MouseWheel != 0.0f) {
                const ImVec2 start(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x,
                                   ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y);
                const ImVec2 local(io.MousePos.x - start.x, io.MousePos.y - start.y);
                const float px = (local.x + ImGui::GetScrollX()) / state.zoom;  // the target pixel under the pointer
                const float py = (local.y + ImGui::GetScrollY()) / state.zoom;
                state.zoom = std::clamp(state.zoom * std::pow(1.15f, io.MouseWheel), 0.1f, 8.0f);
                state.fit = false;
                ImGui::SetScrollX(px * state.zoom - local.x);
                ImGui::SetScrollY(py * state.zoom - local.y);
            }
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Right) || ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
                ImGui::SetScrollX(ImGui::GetScrollX() - io.MouseDelta.x);
                ImGui::SetScrollY(ImGui::GetScrollY() - io.MouseDelta.y);
            }
        }
        const float zoom = state.zoom;
        const ImVec2 size(tw * zoom, th * zoom);
        const ImVec2 origin = ImGui::GetCursorScreenPos();
        ImGui::Image(ImTextureRef(static_cast<ImTextureID>(texture)), size);
        ImDrawList* draw = ImGui::GetWindowDrawList();
        draw->AddRect(ImVec2(origin.x - 1, origin.y - 1), ImVec2(origin.x + size.x + 1, origin.y + size.y + 1),
                      IM_COL32(110, 110, 120, 255));     // the screen's edges, around the black bars
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
                // Dragging it moves it in its scripts (Alt: its dialog), in whole layout units.
                state.dragging = hover.dialog >= 0;
                state.draggingDialog = ImGui::GetIO().KeyAlt;
                state.dragRest[0] = state.dragRest[1] = 0.0f;
            }
        }
        constexpr unsigned kDragMerge = 1;  // one drag on the screen is one undo step
        if (state.dragging) {
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                state.dragging = false;
                if (!state.undo.empty() && state.undo.back().merge == kDragMerge) {
                    state.undo.back().merge = 0;
                }
            } else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                const ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
                const float unit = zoom * engine.UIScale();     // screen pixels per layout unit
                const float tx = std::round(drag.x / unit), ty = std::round(drag.y / unit);
                const float dx = tx - state.dragRest[0], dy = ty - state.dragRest[1];
                if (dx != 0.0f || dy != 0.0f) {
                    if (MoveSelected(engine, state, state.draggingDialog, dx, dy, kDragMerge)) {
                        state.dragRest[0] = tx;
                        state.dragRest[1] = ty;
                    } else {
                        state.dragging = false;
                    }
                }
            }
        }
        if (ImGui::IsWindowFocused() && !ImGui::GetIO().WantTextInput && state.selected.dialog >= 0) {
            const float step = ImGui::GetIO().KeyShift ? 10.0f : 1.0f;
            const bool dialog = ImGui::GetIO().KeyAlt;
            const std::pair<ImGuiKey, ImVec2> keys[] = {{ImGuiKey_LeftArrow, {-step, 0}}, {ImGuiKey_RightArrow, {step, 0}},
                                                        {ImGuiKey_UpArrow, {0, -step}}, {ImGuiKey_DownArrow, {0, step}}};
            for (const auto& [key, move] : keys) {
                if (ImGui::IsKeyPressed(key, true)) {
                    MoveSelected(engine, state, dialog, move.x, move.y, 0);
                }
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
            if (std::sscanf(argv[++i], "%ux%u", &w, &h) == 2) {
                ChooseSize(state, w, h);
                state.exact[0] = w;
                state.exact[1] = h;
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
    for (const std::string& script : state.scripts) {
        state.scriptByKey.emplace(Key(script), script);
    }
    LoadRecord(state);
    LoadAnchors(state);
    for (const std::string& script : toOpen) {
        OpenScript(*engine, state, script);
    }

    std::uint64_t last = SDL_GetTicksNS();
    bool running = true;
    while (running) {
        running = x2::app::PollEvents([&](const SDL_Event& event) { gui->ProcessEvent(event); }, window->Id());
        if (!running && state.Changed()) {
            running = true;     // asked first (QuitPopup)
            state.quitAsked = true;
        }
        if (window->Minimized()) {
            x2::app::WaitForEvents(100);
            continue;
        }
        const std::uint64_t now = SDL_GetTicksNS();
        const float elapsed = std::min(static_cast<float>(static_cast<double>(now - last) / 1e9), 0.1f);
        last = now;

        Poll(*engine, state);
        ReapplyAnchors(*engine, state);
        const unsigned height = state.exact[0] != 0 ? state.exact[1] : kHeights[state.height];
        const unsigned width = state.exact[0] != 0 ? state.exact[0] : WidthFor(height, kAspects[state.aspect].ratio);
        if (!engine->Render(width, height, elapsed, error)) {
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
            ScreenPanel(*engine, *gui, state);
        }
        ImGui::End();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - side, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(side, viewport->WorkSize.y));
        if (ImGui::Begin("Inspector", nullptr, fixed)) {
            InspectorPanel(*engine, state);
        }
        ImGui::End();
        KeyPopup(*engine, state);
        if (QuitPopup(state)) {
            running = false;
        }

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
    if (state.game != nullptr) {
        SDL_DestroyProcess(state.game);     // the game keeps running
    }
    engine.reset();
    device->WaitIdle();
    return 0;
}
