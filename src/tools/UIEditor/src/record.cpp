#include "record.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace x2::ui_editor {

std::string Record::Key(const std::string& script)
{
    std::string key = script;
    const std::size_t slash = key.find_last_of("/\\");
    if (slash != std::string::npos) {
        key.erase(0, slash + 1);
    }
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return key;
}

Record Record::Load(const std::filesystem::path& file)
{
    std::ifstream in(file);
    if (!in) {
        return {};
    }
    return Parse(in);
}

// script, control, type, min_x, min_y, max_x, max_y, dialog_positions ("x,y;x,y"), placed_by_code; '#' comments.
Record Record::Parse(std::istream& in)
{
    Record record;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::vector<std::string> fields;
        std::stringstream split(line);
        for (std::string field; std::getline(split, field, '\t');) {
            fields.push_back(field);
        }
        if (fields.size() < 9) {
            continue;
        }
        Dialog& dialog = record.m_dialogs[Key(fields[0])];
        dialog.placedByCode = dialog.placedByCode || fields[8] == "1";
        std::stringstream positions(fields[7]);
        for (std::string pair; std::getline(positions, pair, ';');) {
            const std::size_t comma = pair.find(',');
            if (comma == std::string::npos) {
                continue;
            }
            const Position p{std::strtof(pair.c_str(), nullptr), std::strtof(pair.c_str() + comma + 1, nullptr)};
            const bool known = std::any_of(dialog.positions.begin(), dialog.positions.end(),
                                           [&](const Position& q) { return q.x == p.x && q.y == p.y; });
            if (!known) {
                dialog.positions.push_back(p);
            }
        }
    }
    return record;
}

const std::vector<Record::Position>* Record::Positions(const std::string& script) const
{
    const auto it = m_dialogs.find(Key(script));
    return it == m_dialogs.end() ? nullptr : &it->second.positions;
}

const Record::Dialog& Record::Of(const std::string& script) const
{
    return m_dialogs.at(Key(script));
}

}  // namespace x2::ui_editor
