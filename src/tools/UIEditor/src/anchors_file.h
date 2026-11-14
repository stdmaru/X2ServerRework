#pragma once

// UIAnchors.lua (decision T44, docs/porting/client-ktdxlib.md#anchors-2026-11-13-clientwidescreen-anchors) as the UI
// editor reads and writes it: the anchors declared for each dialog script. The file is written by hand, with comments
// and controls grouped by line, so an edit changes only the text it must (a value replaced, a field added or
// removed) and leaves the rest of the file as it was.

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace x2::ui_editor {

// One script's entry: ["DLG_x.lua"] = { DIALOG = "LEFT", STRETCH_ZONES = { { 49, 494 } }, CONTROLS = { A = "RIGHT" } }.
struct AnchorEntry {
    std::string dialog;     // "CENTER", "LEFT", "RIGHT" or "STRETCH"; empty when not declared
    std::vector<std::pair<float, float>> zones;
    std::vector<std::pair<std::string, std::string>> controls;   // name -> anchor, in the file's order

    bool Empty() const { return dialog.empty() && zones.empty() && controls.empty(); }
    // The anchor declared for a control, empty when none.
    std::string Control(const std::string& name) const;
    // Declares a control's anchor; an empty anchor removes it.
    void SetControl(const std::string& name, const std::string& anchor);
    bool operator==(const AnchorEntry&) const = default;
};

class AnchorsFile {
public:
    // The file's text; a missing file is empty and gets the file's header when first written.
    static AnchorsFile Parse(std::string text);
    static AnchorsFile Load(const std::filesystem::path& path, std::string& error);
    bool Save(const std::filesystem::path& path, std::string& error);

    // Scripts are matched without folder and case, as the game does.
    AnchorEntry Get(const std::string& script) const;
    bool Has(const std::string& script) const;
    // Makes the script's entry say `entry`: an empty one removes it, a new one is added at the end of UI_ANCHORS.
    void Set(const std::string& script, const AnchorEntry& entry);

    const std::string& Text() const { return m_text; }
    bool Changed() const { return m_changed; }
    // What in the text could not be read (the game reports the same on stderr); empty when all of it was.
    const std::string& Problem() const { return m_problem; }

    // Where the file is in a game folder's client_data (data/), or where a new one goes (data/ui/scripts).
    static std::filesystem::path Find(const std::filesystem::path& gameFolder);

private:
    void Reparse();
    void Replace(std::size_t begin, std::size_t end, const std::string& with);
    void Erase(std::size_t begin, std::size_t end);     // and the line, when only blanks are left on it
    std::string Newline() const;

    // The Lua the file holds, with where each piece is in the text.
    struct Field {
        std::string key;    // empty for a positional field
        std::size_t begin = 0;      // the key's first character (the value's for a positional field)
        std::size_t valueBegin = 0, valueEnd = 0;
        std::size_t sepEnd = 0;     // after the ',' or ';' that follows, or valueEnd
        enum class Kind { String, Number, Table, Other } kind = Kind::Other;
        std::string string;
        double number = 0.0;
        std::size_t table = 0;      // index in m_tables
    };
    struct Table {
        std::size_t open = 0, close = 0;    // the braces
        std::vector<Field> fields;
    };
    struct Script {
        std::string key;    // lowercase file name
        std::size_t begin = 0, end = 0;     // the entry's text, from its [ to after its closing brace
        std::size_t table = 0;              // index of its value in m_tables
        AnchorEntry entry;
    };

    std::string m_text;
    bool m_changed = false;
    std::string m_problem;
    std::size_t m_listEnd = std::string::npos;      // UI_ANCHORS's closing brace
    std::vector<Script> m_scripts;
    std::vector<Table> m_tables;

    friend class Parser;
};

}  // namespace x2::ui_editor
