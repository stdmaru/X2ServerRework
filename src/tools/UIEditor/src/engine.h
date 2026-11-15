#pragma once

// The UI editor's engine (decision T46, docs/tools/ui-editor.md): the client's real CKTDXApp on a window-less D3D9
// device, drawing the opened dialogs into a render target of the chosen size. No X2Lib: the game globals the dialog
// scripts touch get Lua stubs.

#include "anchors_file.h"
#include "live.h"
#include "script_edit.h"
#include "x2/render/render.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace x2::ui_editor {

// Where a script set something: the call on `line` of `script` (CKTDGUIDialog::GetScriptCalls). `shared`: how many
// objects that call set (more than one when it runs in a loop or a function the script calls several times).
struct SourceRef {
    std::string script;         // as the dialog loaded it
    int line = 0;               // 0: not set by a script call the engine saw
    CallKind kind = CallKind::Point;
    int shared = 1;

    bool Known() const { return line > 0; }
};

struct PictureInfo {
    std::string texture;        // file name, empty when none
    std::string key;            // the .tet key, empty when the whole texture is drawn
    float points[4][2] = {};    // left top, right top, left bottom, right bottom (layout units, relative to the control)
    float uv[4][2] = {};        // texture coordinates of the same corners
    float textureSize[2] = {};  // its key's rectangle size in texels (0 when not loaded)
    // Where its size comes from: with USE_TEXTURE_SIZE, the key's size times RESIZE_X/Y plus ADD_SIZE_X/Y; without,
    // the corners its point table gives.
    bool useTextureSize = true;
    float addSize[2] = {}, resize[2] = {1.0f, 1.0f};
    bool shown = true;
    float pixelMin[2] = {}, pixelMax[2] = {};   // its corners' box in render target pixels (anchors and stretch applied)
    SourceRef point;            // its SetPoint
    SourceRef texSource;        // its SetTex(file, key)
};

struct StringInfo {
    std::string text;           // as the dialog holds it now; empty when the game's code fills it at run time
    int stringId = -1;          // the string table index the script named (STR_ID_*), -1 when none
    float pos[2] = {};          // layout units, relative to the control
    int font = 0;               // the font index (XUF_*)
    unsigned sortFlag = 0;      // DT_* alignment flags
    float color[4] = {};        // r, g, b, a
    SourceRef source;           // its AddString
};

struct ControlInfo {
    int index = 0;              // in the dialog's control list
    std::string name;           // "#<index>" when unnamed, as the anchor recorder names it
    bool named = false;
    int type = 0;               // CKTDGUIControl::CONTROL_TYPE
    std::string typeName;
    std::string anchor;         // "CENTER" (also when not declared), "LEFT", "RIGHT", "STRETCH"
    bool anchorDeclared = false;
    bool anchorFromScript = false;  // the script's own SetAnchor, which UIAnchors.lua does not override
    bool shown = true;
    float offset[2] = {};       // the control's offset in the dialog (layout units)
    bool drawn = false;         // drew something in the last frame
    float drawnMin[2] = {}, drawnMax[2] = {};   // what it drew in the last frame, in layout units relative to the dialog
    float pixelMin[2] = {}, pixelMax[2] = {};   // the same box in render target pixels (anchors and stretch applied)
    std::vector<PictureInfo> pictures;
    std::vector<StringInfo> strings;
    std::vector<SourceRef> points;  // the point tables set on the control itself (a button's normal, over, down, ...)
};

struct DialogInfo {
    std::string script;         // as opened (DLG_*.lua)
    std::string name;           // the dialog's SetName
    float pos[2] = {};          // layout position under the dialog's anchor (GetPos)
    float canvasPos[2] = {};    // where it is drawn, in layout units (GetCanvasPos)
    std::string anchor;
    bool anchorDeclared = false;
    bool anchorFromScript = false;
    std::vector<std::pair<float, float>> stretchZones;
    bool placedByCode = false;
    bool loaded = false;        // the script ran without error
    SourceRef posSource;        // its SetPos (the last one)
    std::vector<ControlInfo> controls;
};

struct EngineOptions {
    std::filesystem::path gameFolder;   // the client folder: the .kom archives and, with dev tools, data/ (client_data)
    bool mountArchives = true;          // false: loose files only (tests)
};

class Engine {
public:
    // `render` must outlive the engine. Changes the working folder to the game folder, as the client runs there.
    static std::unique_ptr<Engine> Create(render::Device& render, const EngineOptions& options, std::string& error);
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    // Opens a dialog script (DLG_*.lua, found by name in client_data or the archives) on top of the open ones. Returns
    // its index in Dialogs(), or -1 (error set) when the script did not load.
    int OpenDialog(const std::string& script, std::string& error);
    void CloseDialog(int index);
    void CloseAll();
    std::size_t DialogCount() const;
    // The script an open dialog was opened from, as given to OpenDialog.
    const std::string& DialogScript(int index) const;
    // Moves an open dialog to a layout position under its anchor, as the game's SetPos.
    void SetDialogPos(int index, float x, float y);
    void SetDialogShown(int index, bool shown);
    // Loads another script into an open dialog, as the game's code does (CKTDGUIDialog::OpenScriptFile).
    bool LoadScriptInto(int index, const std::string& script, std::string& error);
    const std::vector<std::string>& DialogExtraScripts(int index) const;
    // Puts what the running game shows into an open dialog built from the same scripts (watch game): position,
    // layer, shown flags, and every static as the game holds it (pictures and texts code added, removed or changed;
    // statics code added). Controls of other types code added are left out.
    void ApplyLive(int index, const LiveSnapshot::Dialog& live);
    // Puts an entry of UIAnchors.lua into an open dialog, as the game does after its script loads, replacing what
    // `previous` declared: an anchor `previous` had and `entry` has not goes back to none. Anchors the script declares
    // itself stay (they win in the game). Control names the dialog lacks are skipped.
    void ApplyAnchors(int index, const AnchorEntry& entry, const AnchorEntry& previous);

    // Script edits not saved yet (docs/tools/ui-editor.md#editing-positions): a script with text set here runs from it
    // instead of its file, in every dialog loaded after. nullptr goes back to the file.
    void SetScriptText(const std::string& script, const std::string* text);
    // The script's text: the one set, or its file's (with its BOM). False with `error` when there is no such file.
    bool ScriptText(const std::string& script, std::string& text, std::string& error) const;
    // Builds an open dialog again from its scripts (after SetScriptText), keeping where it is drawn, whether it is
    // shown and the game's state last put into it (ApplyLive), moved by what the edit moved in its scripts.
    bool ReloadDialog(int index, std::string& error);
    // Whether an open dialog was built from the script (first or extra).
    bool DialogUses(int index, const std::string& script) const;

    // .tet edits not saved yet (docs/tools/ui-editor.md#editing-textures-and-uv). The bytes of a texture's .tet: its
    // file's, from client_data or the archives, or the ones set here. False with `error` when it has none.
    bool TetBytes(const std::string& texture, std::string& bytes, std::string& error) const;
    // Makes a texture's UV rectangles these, a whole .tet file, in the pictures built after (ReloadDialog builds
    // the open ones again). False with `error` when the bytes are not a .tet or the texture does not load.
    bool SetTet(const std::string& texture, const std::string& bytes, std::string& error);
    // Whether an open dialog has a picture of the texture.
    bool DialogUsesTexture(int index, const std::string& texture) const;

    // A texture as the game loads it, to draw in the editor (ImGuiLayer::TextureId), and its size in texels, those of
    // its .tet. Held loaded until the engine goes. False with `error` when it does not load.
    struct TextureImage {
        render::Texture texture;
        unsigned width = 0, height = 0;
    };
    bool LoadTextureImage(const std::string& texture, TextureImage& image, std::string& error);

    // Draws every open dialog into the render target, resized to width x height first. False with error set if the
    // frame failed. The texture stays valid until the next Render with another size or the engine is destroyed.
    bool Render(unsigned width, unsigned height, float elapsed, std::string& error);
    render::Texture Target() const;
    unsigned TargetWidth() const;
    unsigned TargetHeight() const;

    // What the open dialogs hold, after the last Render (drawn boxes come from that frame).
    std::vector<DialogInfo> Inspect() const;

    // The UI canvas of the last Render: scale (pixels per layout unit) and where layout (0, 0) of the centered column is.
    float UIScale() const;
    float UIOriginX() const;
    float UIOriginY() const;

private:
    struct State;
    explicit Engine(std::unique_ptr<State> state);
    std::unique_ptr<State> m_state;
};

// The dialog scripts (DLG_*.lua) under the game folder's data/ (client_data), sorted by name, without folders.
std::vector<std::string> FindDialogScripts(const std::filesystem::path& gameFolder);
// A script's loose file under the game folder's data/, matched without case; empty when there is none.
std::filesystem::path FindLooseScript(const std::filesystem::path& gameFolder, const std::string& script);

}  // namespace x2::ui_editor
