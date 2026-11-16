#pragma once

// The client's live UI snapshot (UILive.tsv, written with X2_UI_LIVE=1; CKTDGUIAnchorRecorder::NoteLive): the shown
// dialogs drawn from scripts, back to front, with what the game's code changed in them. The editor's "watch game"
// mirrors it (docs/tools/ui-editor.md).

#include <filesystem>
#include <istream>
#include <string>
#include <vector>

namespace x2::ui_editor {

struct LiveSnapshot {
    struct Control {
        int index = 0;
        bool shown = true;
        int type = -1;              // CKTDGUIControl::CONTROL_TYPE, -1 when not written
        int anchor = -1;            // UIAnchor the control declares, -1: none (or not written)
        float offset[2] = {};       // what code moved it by (SetOffsetPos), layout units
        bool hasOffset = false;     // written (older snapshots do not)
    };
    struct Picture {
        int control = 0, index = 0;
        bool shown = true;
        std::string texture, key;   // empty texture: none
        float points[4][2] = {};    // left top, right top, left bottom, right bottom (layout units, as the picture holds them)
        float uv[4][2] = {};
        float color[4] = {1, 1, 1, 1};
        bool full = false;          // points, uv and color written
    };
    struct Text {
        int control = 0, index = 0;
        std::string text;
        int font = 0;
        unsigned sortFlag = 0;
        float pos[2] = {};
        float color[4] = {1, 1, 1, 1};
        bool full = false;          // font, alignment, position and color written
    };
    struct Dialog {
        std::string script;         // lowercase, without folder
        std::vector<std::string> extraScripts;   // loaded into it by code after the first, in order
        float pos[2] = {};          // layout units under the dialog's anchor
        int layer = 0;
        bool front = false;
        int anchor = -1;            // UIAnchor in effect in the game (code sets some, T44), -1 when not written
        std::string placement;      // the named placement the code put it at (ApplyPlacement, T52); empty: none
        std::vector<Control> controls;
        std::vector<Picture> pictures;
        std::vector<Text> texts;

        // What tells two of the game's dialogs apart for the editor: the scripts it was built from.
        std::string Key() const;
    };

    std::string scene;              // XS_*, empty before the first state
    unsigned screen[2] = {};        // the game's resolution, 0 when not written
    std::vector<Dialog> dialogs;

    // An empty snapshot (no dialogs) when the file cannot be read.
    static LiveSnapshot Load(const std::filesystem::path& file);
    static LiveSnapshot Parse(std::istream& in);
};

}  // namespace x2::ui_editor
