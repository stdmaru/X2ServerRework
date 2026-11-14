#pragma once

// The anchor recorder's file (UIAnchorsRecord.tsv, docs/tools/ui-anchors.md), read for where each dialog was drawn in
// the game: until a screen's layout positions move into its scripts (T46), the editor places a dialog the game's code
// positions at a recorded position.

#include <filesystem>
#include <set>
#include <istream>
#include <string>
#include <unordered_map>
#include <vector>

namespace x2::ui_editor {

class Record {
public:
    struct Position {
        float x = 0.0f, y = 0.0f;   // layout units under the dialog's anchor (GetPos)
    };
    struct Dialog {
        std::vector<Position> positions;    // in the order first seen
        bool placedByCode = false;
        std::vector<std::string> scenes;    // the game states it was seen in (XS_*), sorted
    };

    // An empty record when the file does not exist.
    static Record Load(const std::filesystem::path& file);
    static Record Parse(std::istream& in);

    // Scripts are matched without folder and case, as the recorder writes them. Null when never recorded.
    const std::vector<Position>* Positions(const std::string& script) const;
    // Only for a recorded script (Positions() not null).
    const Dialog& Of(const std::string& script) const;
    std::size_t Size() const { return m_dialogs.size(); }
    // Every scene recorded, sorted; and the scripts (lowercase, without folder) seen in one, sorted.
    std::vector<std::string> Scenes() const;
    std::vector<std::string> ScriptsIn(const std::string& scene) const;

private:
    static std::string Key(const std::string& script);
    std::unordered_map<std::string, Dialog> m_dialogs;
};

}  // namespace x2::ui_editor
