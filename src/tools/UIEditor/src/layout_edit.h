#pragma once

// Layout edits over many pieces of a dialog (docs/tools/ui-editor.md#resizing-a-dialog): the script calls they change
// and how, planned from what the engine inspected; the editor makes them (x2::ui_editor::MoveCall, ResizeCall).

#include "engine.h"

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

namespace x2::ui_editor {

// An edit of the script call behind something: moves what it places by (move) layout units, or, resize, moves its
// edges by (left, top, right, bottom) so it grows (x2::ui_editor::MoveCall, ResizeCall).
struct CallEdit {
    SourceRef ref;
    bool resize = false;
    float d[4] = {};    // move: dx, dy; resize: left, top, right, bottom

    bool SameAs(const CallEdit& other) const
    {
        return resize == other.resize && std::equal(std::begin(d), std::end(d), std::begin(other.d));
    }
};

// A picture's box in layout units relative to its control: min x, min y, max x, max y.
void PictureBox(const PictureInfo& picture, float box[4]);

// What resizing a dialog at a line does: the edits, and the pieces for the inspector and the screen.
struct CutPlan {
    std::vector<CallEdit> edits;
    struct Piece {
        std::string name;
        float pixelMin[2] = {}, pixelMax[2] = {};
    };
    std::vector<Piece> grows, moves, skipped;   // skipped: crossed by the line but not a picture, or not from a script
    int stays = 0;
};

// Plans growing `dialog` by `grow` layout units at the line `at` (relative to the dialog, before anchors: the scripts'
// numbers): a picture the line crosses grows (its SetPoint), a picture, text or other control past it moves.
CutPlan PlanCut(const DialogInfo& dialog, bool vertical, float at, float grow);

// The extent of what a dialog draws, in layout units relative to it: min x, min y, max x, max y. False when nothing.
bool DialogExtent(const DialogInfo& dialog, float box[4]);

}  // namespace x2::ui_editor
