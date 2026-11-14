#include "live.h"

#include <cstdlib>
#include <fstream>
#include <sstream>

namespace x2::ui_editor {

namespace {

std::string Unescape(const std::string& in)
{
    std::string out;
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i) {
        if (in[i] != '\\' || i + 1 == in.size()) {
            out += in[i];
            continue;
        }
        switch (in[++i]) {
        case 't': out += '\t'; break;
        case 'n': out += '\n'; break;
        case 'r': out += '\r'; break;
        default: out += in[i]; break;
        }
    }
    return out;
}

int Int(const std::string& field)
{
    return static_cast<int>(std::strtol(field.c_str(), nullptr, 10));
}

}  // namespace

LiveSnapshot LiveSnapshot::Load(const std::filesystem::path& file)
{
    std::ifstream in(file, std::ios::binary);
    if (!in) {
        return {};
    }
    return Parse(in);
}

// One record per line, tab separated; a control, picture or text belongs to the dialog above it.
LiveSnapshot LiveSnapshot::Parse(std::istream& in)
{
    LiveSnapshot snapshot;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        std::vector<std::string> f;
        std::stringstream split(line);
        for (std::string field; std::getline(split, field, '\t');) {
            f.push_back(field);
        }
        if (line.size() > 0 && line.back() == '\t') {
            f.emplace_back();   // getline drops a trailing empty field
        }
        if (f.empty()) {
            continue;
        }
        const std::string& kind = f[0];
        Dialog* dialog = snapshot.dialogs.empty() ? nullptr : &snapshot.dialogs.back();
        if (kind == "scene" && f.size() >= 2) {
            snapshot.scene = f[1];
        } else if (kind == "screen" && f.size() >= 3) {
            snapshot.screen[0] = static_cast<unsigned>(Int(f[1]));
            snapshot.screen[1] = static_cast<unsigned>(Int(f[2]));
        } else if (kind == "dialog" && f.size() >= 6) {
            Dialog d;
            d.script = f[1];
            d.pos[0] = std::strtof(f[2].c_str(), nullptr);
            d.pos[1] = std::strtof(f[3].c_str(), nullptr);
            d.layer = Int(f[4]);
            d.front = f[5] == "1";
            snapshot.dialogs.push_back(std::move(d));
        } else if (kind == "control" && dialog != nullptr && f.size() >= 3) {
            dialog->controls.push_back({Int(f[1]), f[2] == "1"});
        } else if (kind == "picture" && dialog != nullptr && f.size() >= 4) {
            Picture p;
            p.control = Int(f[1]);
            p.index = Int(f[2]);
            p.shown = f[3] == "1";
            p.texture = f.size() > 4 ? Unescape(f[4]) : std::string();
            p.key = f.size() > 5 ? Unescape(f[5]) : std::string();
            dialog->pictures.push_back(std::move(p));
        } else if (kind == "text" && dialog != nullptr && f.size() >= 3) {
            dialog->texts.push_back({Int(f[1]), Int(f[2]), f.size() > 3 ? Unescape(f[3]) : std::string()});
        }
    }
    return snapshot;
}

}  // namespace x2::ui_editor
