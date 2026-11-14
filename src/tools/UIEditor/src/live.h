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
    };
    struct Picture {
        int control = 0, index = 0;
        bool shown = true;
        std::string texture, key;   // empty texture: none
    };
    struct Text {
        int control = 0, index = 0;
        std::string text;
    };
    struct Dialog {
        std::string script;         // lowercase, without folder
        float pos[2] = {};          // layout units under the dialog's anchor
        int layer = 0;
        bool front = false;
        std::vector<Control> controls;
        std::vector<Picture> pictures;
        std::vector<Text> texts;
    };

    std::string scene;              // XS_*, empty before the first state
    unsigned screen[2] = {};        // the game's resolution, 0 when not written
    std::vector<Dialog> dialogs;

    // An empty snapshot (no dialogs) when the file cannot be read.
    static LiveSnapshot Load(const std::filesystem::path& file);
    static LiveSnapshot Parse(std::istream& in);
};

}  // namespace x2::ui_editor
