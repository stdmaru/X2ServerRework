#include "layout_edit.h"

namespace x2::ui_editor {

void PictureBox(const PictureInfo& picture, float box[4])
{
    box[0] = box[2] = picture.points[0][0];
    box[1] = box[3] = picture.points[0][1];
    for (const auto& point : picture.points) {
        box[0] = std::min(box[0], point[0]);
        box[1] = std::min(box[1], point[1]);
        box[2] = std::max(box[2], point[0]);
        box[3] = std::max(box[3], point[1]);
    }
}

CutPlan PlanCut(const DialogInfo& dialog, bool vertical, float at, float grow)
{
    CutPlan plan;
    const int axis = vertical ? 0 : 1;
    const auto piece = [](const std::string& name, const float lo[2], const float hi[2]) {
        CutPlan::Piece p;
        p.name = name;
        p.pixelMin[0] = lo[0], p.pixelMin[1] = lo[1], p.pixelMax[0] = hi[0], p.pixelMax[1] = hi[1];
        return p;
    };
    for (const ControlInfo& control : dialog.controls) {
        if (!control.pictures.empty() || !control.strings.empty()) {   // a static
            for (std::size_t p = 0; p < control.pictures.size(); ++p) {
                const PictureInfo& picture = control.pictures[p];
                float box[4];
                PictureBox(picture, box);
                const float lo = box[axis] + control.offset[axis], hi = box[axis + 2] + control.offset[axis];
                const std::string name = control.name + " / " + std::to_string(p + 1) + " " + picture.key;
                if (hi <= at) {
                    ++plan.stays;
                } else if (!picture.point.Known()) {
                    plan.skipped.push_back(piece(name + " (no SetPoint)", picture.pixelMin, picture.pixelMax));
                } else if (lo >= at) {
                    plan.edits.push_back({picture.point, false, {vertical ? grow : 0.0f, vertical ? 0.0f : grow, 0, 0}});
                    plan.moves.push_back(piece(name, picture.pixelMin, picture.pixelMax));
                } else {
                    plan.edits.push_back({picture.point, true, {0, 0, vertical ? grow : 0.0f, vertical ? 0.0f : grow}});
                    plan.grows.push_back(piece(name, picture.pixelMin, picture.pixelMax));
                }
            }
            for (std::size_t t = 0; t < control.strings.size(); ++t) {
                const auto& string = control.strings[t];
                if (string.pos[axis] + control.offset[axis] < at) {
                    ++plan.stays;
                } else if (string.source.Known()) {
                    plan.edits.push_back({string.source, false, {vertical ? grow : 0.0f, vertical ? 0.0f : grow, 0, 0}});
                    plan.moves.push_back(piece(control.name + " / text " + std::to_string(t + 1), control.pixelMin,
                                               control.pixelMax));
                } else {
                    plan.skipped.push_back(piece(control.name + " / text " + std::to_string(t + 1) + " (no AddString)",
                                                 control.pixelMin, control.pixelMax));
                }
            }
            continue;
        }
        // Other controls (buttons, lists, ...) move whole when past the line; crossed, they are left as they are.
        if (!control.drawn) {
            continue;
        }
        const float lo = control.drawnMin[axis], hi = control.drawnMax[axis];
        if (hi <= at) {
            ++plan.stays;
        } else if (lo < at) {
            plan.skipped.push_back(piece(control.name + " (crossed: not a picture)", control.pixelMin, control.pixelMax));
        } else if (control.points.empty()) {
            plan.skipped.push_back(piece(control.name + " (no point table)", control.pixelMin, control.pixelMax));
        } else {
            for (const SourceRef& ref : control.points) {
                plan.edits.push_back({ref, false, {vertical ? grow : 0.0f, vertical ? 0.0f : grow, 0, 0}});
            }
            plan.moves.push_back(piece(control.name, control.pixelMin, control.pixelMax));
        }
    }
    return plan;
}

// The extent of what a dialog draws, in layout units relative to it: min x, min y, max x, max y. False when nothing.
bool DialogExtent(const DialogInfo& dialog, float box[4])
{
    bool any = false;
    for (const ControlInfo& control : dialog.controls) {
        if (!control.drawn) {
            continue;
        }
        if (!any) {
            box[0] = control.drawnMin[0], box[1] = control.drawnMin[1], box[2] = control.drawnMax[0], box[3] = control.drawnMax[1];
            any = true;
        }
        box[0] = std::min(box[0], control.drawnMin[0]);
        box[1] = std::min(box[1], control.drawnMin[1]);
        box[2] = std::max(box[2], control.drawnMax[0]);
        box[3] = std::max(box[3], control.drawnMax[1]);
    }
    return any;
}

}  // namespace x2::ui_editor
