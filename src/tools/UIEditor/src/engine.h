#pragma once

// The UI editor's engine (decision T46, docs/tools/ui-editor.md): the client's real CKTDXApp on a window-less D3D9
// device, drawing the opened dialogs into a render target of the chosen size. No X2Lib: the game globals the dialog
// scripts touch get Lua stubs.

#include "live.h"
#include "x2/render/render.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace x2::ui_editor {

struct PictureInfo {
    std::string texture;        // file name, empty when none
    std::string key;            // the .tet key, empty when the whole texture is drawn
    float points[4][2] = {};    // left top, right top, left bottom, right bottom (layout units, relative to the control)
    float uv[4][2] = {};        // texture coordinates of the same corners
    float textureSize[2] = {};  // the texture's size in texels (0 when not loaded)
    bool shown = true;
};

struct StringInfo {
    std::string text;           // as the dialog holds it now; empty when the game's code fills it at run time
    int stringId = -1;          // the string table index the script named (STR_ID_*), -1 when none
    float pos[2] = {};          // layout units, relative to the control
    int font = 0;               // the font index (XUF_*)
    unsigned sortFlag = 0;      // DT_* alignment flags
    float color[4] = {};        // r, g, b, a
};

struct ControlInfo {
    int index = 0;              // in the dialog's control list
    std::string name;           // "#<index>" when unnamed, as the anchor recorder names it
    bool named = false;
    int type = 0;               // CKTDGUIControl::CONTROL_TYPE
    std::string typeName;
    std::string anchor;         // "CENTER" (also when not declared), "LEFT", "RIGHT", "STRETCH"
    bool anchorDeclared = false;
    bool shown = true;
    float offset[2] = {};       // the control's offset in the dialog (layout units)
    bool drawn = false;         // drew something in the last frame
    float drawnMin[2] = {}, drawnMax[2] = {};   // what it drew in the last frame, in layout units relative to the dialog
    float pixelMin[2] = {}, pixelMax[2] = {};   // the same box in render target pixels (anchors and stretch applied)
    std::vector<PictureInfo> pictures;
    std::vector<StringInfo> strings;
};

struct DialogInfo {
    std::string script;         // as opened (DLG_*.lua)
    std::string name;           // the dialog's SetName
    float pos[2] = {};          // layout position under the dialog's anchor (GetPos)
    float canvasPos[2] = {};    // where it is drawn, in layout units (GetCanvasPos)
    std::string anchor;
    bool anchorDeclared = false;
    bool placedByCode = false;
    bool loaded = false;        // the script ran without error
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

}  // namespace x2::ui_editor
