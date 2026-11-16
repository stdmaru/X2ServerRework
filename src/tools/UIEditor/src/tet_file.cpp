#include "tet_file.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string_view>
#include <system_error>

namespace x2::ui_editor {

namespace {

constexpr std::uint32_t kMagic = 0x20544554;    // "TET "
constexpr std::uint32_t kVersion = 0x0100;
constexpr std::size_t kHeader = 12;
constexpr std::size_t kRect = 40;               // 10 floats

std::uint32_t ReadU32(const std::string& b, std::size_t at)
{
    std::uint32_t v = 0;
    std::memcpy(&v, b.data() + at, sizeof(v));
    return v;
}

void AppendU32(std::string& b, std::uint32_t v)
{
    b.append(reinterpret_cast<const char*>(&v), sizeof(v));
}

// UTF-16 <-> UTF-8, for the keys (ASCII in practice).
std::string Utf8(const std::u16string& s)
{
    std::string out;
    for (std::size_t i = 0; i < s.size(); ++i) {
        std::uint32_t c = s[i];
        if (c >= 0xD800 && c < 0xDC00 && i + 1 < s.size() && s[i + 1] >= 0xDC00 && s[i + 1] < 0xE000) {
            c = 0x10000 + ((c - 0xD800) << 10) + (s[++i] - 0xDC00);
        }
        if (c < 0x80) {
            out += static_cast<char>(c);
        } else if (c < 0x800) {
            out += static_cast<char>(0xC0 | (c >> 6));
            out += static_cast<char>(0x80 | (c & 0x3F));
        } else if (c < 0x10000) {
            out += static_cast<char>(0xE0 | (c >> 12));
            out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (c & 0x3F));
        } else {
            out += static_cast<char>(0xF0 | (c >> 18));
            out += static_cast<char>(0x80 | ((c >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (c & 0x3F));
        }
    }
    return out;
}

std::u16string Utf16(const std::string& s)
{
    std::u16string out;
    for (std::size_t i = 0; i < s.size();) {
        const auto b = static_cast<unsigned char>(s[i]);
        std::uint32_t c = b;
        int extra = b >= 0xF0 ? 3 : b >= 0xE0 ? 2 : b >= 0xC0 ? 1 : 0;
        c = extra == 3 ? b & 0x07 : extra == 2 ? b & 0x0F : extra == 1 ? b & 0x1F : b;
        ++i;
        for (; extra > 0 && i < s.size(); --extra, ++i) {
            c = (c << 6) | (static_cast<unsigned char>(s[i]) & 0x3F);
        }
        if (c >= 0x10000) {
            c -= 0x10000;
            out += static_cast<char16_t>(0xD800 + (c >> 10));
            out += static_cast<char16_t>(0xDC00 + (c & 0x3FF));
        } else {
            out += static_cast<char16_t>(c);
        }
    }
    return out;
}

bool KeyLess(const TetFile::Entry& a, const TetFile::Entry& b)
{
    return Utf16(a.key) < Utf16(b.key);
}

std::string Lower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

// Whether `line` has `word` between quotes ("word" or 'word'); both are lowercase.
bool HasQuoted(const std::string& line, const std::string& word)
{
    for (std::size_t at = line.find(word); at != std::string::npos; at = line.find(word, at + 1)) {
        const std::size_t end = at + word.size();
        if (at > 0 && end < line.size() && (line[at - 1] == '"' || line[at - 1] == '\'') && line[end] == line[at - 1]) {
            return true;
        }
    }
    return false;
}

}  // namespace

bool TetRect::Upright() const
{
    return leftBottom[0] == leftTop[0] && leftBottom[1] == rightBottom[1] && rightTop[0] == rightBottom[0] &&
           rightTop[1] == leftTop[1];
}

void TetRect::Move(float dx, float dy)
{
    for (float* corner : {leftTop, rightBottom, leftBottom, rightTop}) {
        corner[0] += dx;
        corner[1] += dy;
    }
}

bool TetRect::Resize(float dw, float dh)
{
    if (!Upright()) {
        return false;
    }
    rightBottom[0] += dw;
    rightBottom[1] += dh;
    rightTop[0] += dw;
    leftBottom[1] += dh;
    // As the client's AddRect.
    size[0] = std::fabs(leftTop[0] - rightTop[0]);
    size[1] = std::fabs(leftTop[1] - leftBottom[1]);
    return true;
}

bool TetFile::Parse(const std::string& bytes, TetFile& file, std::string& error)
{
    file.m_entries.clear();
    if (bytes.size() < kHeader || ReadU32(bytes, 0) != kMagic || ReadU32(bytes, 4) != kVersion) {
        error = "not a .tet file";
        return false;
    }
    const std::size_t count = ReadU32(bytes, 8);
    if (bytes.size() < kHeader + count * (4 + kRect)) {
        error = ".tet file cut short";
        return false;
    }
    for (std::size_t i = 0; i < count; ++i) {
        const std::size_t offset = ReadU32(bytes, kHeader + 4 * i);
        std::uint16_t length = 0;
        if (offset == 0 || offset % 2 != 0 || offset + 2 > bytes.size()) {
            error = ".tet key " + std::to_string(i) + " has no name";
            return false;
        }
        std::memcpy(&length, bytes.data() + offset, sizeof(length));
        if (offset + 2 + (std::size_t{length} + 1) * 2 > bytes.size()) {
            error = ".tet key " + std::to_string(i) + " cut short";
            return false;
        }
        std::u16string key(length, u'\0');
        std::memcpy(key.data(), bytes.data() + offset + 2, std::size_t{length} * 2);
        Entry entry;
        entry.key = Utf8(key);
        float v[10];
        std::memcpy(v, bytes.data() + kHeader + 4 * count + kRect * i, sizeof(v));
        TetRect& r = entry.rect;
        r.leftTop[0] = v[0], r.leftTop[1] = v[1];
        r.rightBottom[0] = v[2], r.rightBottom[1] = v[3];
        r.leftBottom[0] = v[4], r.leftBottom[1] = v[5];
        r.rightTop[0] = v[6], r.rightTop[1] = v[7];
        r.size[0] = v[8], r.size[1] = v[9];
        file.m_entries.push_back(std::move(entry));
    }
    // The client finds keys by binary search: a file out of order is one it misreads, written sorted.
    std::stable_sort(file.m_entries.begin(), file.m_entries.end(), KeyLess);
    return true;
}

std::string TetFile::Bytes() const
{
    std::string out;
    AppendU32(out, kMagic);
    AppendU32(out, kVersion);
    AppendU32(out, static_cast<std::uint32_t>(m_entries.size()));
    std::size_t offset = kHeader + m_entries.size() * (4 + kRect);
    std::vector<std::u16string> keys;
    for (const Entry& entry : m_entries) {
        keys.push_back(Utf16(entry.key));
        AppendU32(out, static_cast<std::uint32_t>(offset));
        offset += 2 + (keys.back().size() + 1) * 2;
    }
    for (const Entry& entry : m_entries) {
        const TetRect& r = entry.rect;
        const float v[10] = {r.leftTop[0],    r.leftTop[1],    r.rightBottom[0], r.rightBottom[1], r.leftBottom[0],
                             r.leftBottom[1], r.rightTop[0],   r.rightTop[1],    r.size[0],        r.size[1]};
        out.append(reinterpret_cast<const char*>(v), sizeof(v));
    }
    for (const std::u16string& key : keys) {
        const auto length = static_cast<std::uint16_t>(key.size());
        out.append(reinterpret_cast<const char*>(&length), sizeof(length));
        out.append(reinterpret_cast<const char*>(key.data()), key.size() * 2);
        out.append(2, '\0');
    }
    return out;
}

const TetRect* TetFile::Find(const std::string& key) const
{
    const std::string upper = UpperKey(key);
    for (const Entry& entry : m_entries) {
        if (entry.key == upper) {
            return &entry.rect;
        }
    }
    return nullptr;
}

bool TetFile::Set(const std::string& key, const TetRect& rect)
{
    const std::string upper = UpperKey(key);
    for (Entry& entry : m_entries) {
        if (entry.key == upper) {
            entry.rect = rect;
            return true;
        }
    }
    return false;
}

bool TetFile::Add(const std::string& key, const TetRect& rect, std::string& error)
{
    if (!ValidKeyName(key)) {
        error = "a key is letters, digits and underscores";
        return false;
    }
    if (Find(key) != nullptr) {
        error = "the texture has a key " + UpperKey(key) + " already";
        return false;
    }
    Entry entry{UpperKey(key), rect};
    m_entries.insert(std::upper_bound(m_entries.begin(), m_entries.end(), entry, KeyLess), std::move(entry));
    return true;
}

std::string TetName(const std::string& texture)
{
    return texture.size() > 3 ? texture.substr(0, texture.size() - 3) + "TET" : std::string();
}

bool ValidKeyName(const std::string& key)
{
    return !key.empty() && std::all_of(key.begin(), key.end(), [](unsigned char c) {
        return std::isalnum(c) != 0 || c == '_';
    });
}

std::string UpperKey(const std::string& key)
{
    std::string upper = key;
    std::transform(upper.begin(), upper.end(), upper.begin(),
                   [](unsigned char c) { return static_cast<char>(c < 0x80 ? std::toupper(c) : c); });
    return upper;
}

KeyUseIndex KeyUseIndex::Build(const std::filesystem::path& gameFolder)
{
    KeyUseIndex index;
    const std::filesystem::path data = gameFolder / "data";
    std::error_code ec;
    for (auto it = std::filesystem::recursive_directory_iterator(
             data, std::filesystem::directory_options::skip_permission_denied, ec);
         !ec && it != std::filesystem::recursive_directory_iterator(); it.increment(ec)) {
        const std::string name = it->path().filename().string();
        if (it->is_directory(ec)) {
            if (!name.empty() && name[0] == '.') {
                it.disable_recursion_pending();     // client_data's .kom/
            }
            continue;
        }
        if (Lower(it->path().extension().string()) != ".lua") {
            continue;
        }
        std::ifstream in(it->path(), std::ios::binary);
        const std::string text = Lower(std::string(std::istreambuf_iterator<char>(in), {}));
        std::size_t script = SIZE_MAX;
        int line = 1;
        for (std::size_t begin = 0; begin < text.size(); ++line) {
            std::size_t end = text.find('\n', begin);
            end = end == std::string::npos ? text.size() : end;
            const std::string_view l(text.data() + begin, end - begin);
            begin = end + 1;
            if (l.find('.') == std::string_view::npos) {
                continue;
            }
            // Every quoted name with a '.' on the line (a file), each once.
            std::vector<std::string> names;
            for (std::size_t q = 0; q < l.size(); ++q) {
                if (l[q] != '"' && l[q] != '\'') {
                    continue;
                }
                const std::size_t close = l.find(l[q], q + 1);
                if (close == std::string_view::npos) {
                    break;
                }
                const std::string_view quoted = l.substr(q + 1, close - q - 1);
                if (quoted.find('.') != std::string_view::npos &&
                    std::find(names.begin(), names.end(), quoted) == names.end()) {
                    names.emplace_back(quoted);
                }
                q = close;
            }
            if (names.empty()) {
                continue;
            }
            if (script == SIZE_MAX) {
                script = index.m_scripts.size();
                index.m_scripts.push_back(std::filesystem::relative(it->path(), data, ec));
            }
            for (const std::string& n : names) {
                index.m_lines[n].push_back({script, line, std::string(l)});
            }
        }
    }
    return index;
}

std::vector<KeyUse> KeyUseIndex::Find(const std::string& texture, const std::string& key) const
{
    std::vector<KeyUse> uses;
    const auto found = m_lines.find(Lower(texture));
    if (found == m_lines.end()) {
        return uses;
    }
    const std::string k = Lower(key);
    for (const Line& line : found->second) {
        if (HasQuoted(line.text, k)) {
            uses.push_back({m_scripts[line.script], line.line});
        }
    }
    std::sort(uses.begin(), uses.end(), [](const KeyUse& a, const KeyUse& b) {
        return a.script != b.script ? a.script < b.script : a.line < b.line;
    });
    return uses;
}

std::vector<KeyUse> FindKeyUses(const std::filesystem::path& gameFolder, const std::string& texture,
                                const std::string& key)
{
    return KeyUseIndex::Build(gameFolder).Find(texture, key);
}

}  // namespace x2::ui_editor
