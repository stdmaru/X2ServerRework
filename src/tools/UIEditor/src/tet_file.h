#pragma once

// A texture's .tet file (docs/tools/ui-editor.md#editing-textures-and-uv): the UV rectangle of each key the scripts
// name in SetTex(file, key), in texels. The editor reads it, changes rectangles or adds keys, and writes it back as
// the client reads it (CKTDXDeviceTET_Preprocessing): header (magic "TET ", version 0x100, count), the keys' offsets,
// the rectangles, then the keys (WORD length, UTF-16LE, a null), keys sorted by code unit for the client's binary search.

#include <filesystem>
#include <string>
#include <vector>

namespace x2::ui_editor {

struct TetRect {
    // The corners of the texture the picture's corners take, in texels, and the size USE_TEXTURE_SIZE gives it.
    float leftTop[2] = {}, rightBottom[2] = {}, leftBottom[2] = {}, rightTop[2] = {}, size[2] = {};

    // Not rotated (RotateRect): left bottom and right top are the other two corners of left top - right bottom.
    bool Upright() const;
    // Moves every corner by (dx, dy) texels.
    void Move(float dx, float dy);
    // Grows an upright rectangle by (dw, dh) texels to the right and down; false, unchanged, when it is rotated.
    bool Resize(float dw, float dh);

    bool operator==(const TetRect&) const = default;
};

class TetFile {
public:
    struct Entry {
        std::string key;    // uppercase, as the client looks keys up
        TetRect rect;
    };

    // Reads a .tet's bytes; false with `error` when they are not one.
    static bool Parse(const std::string& bytes, TetFile& file, std::string& error);
    std::string Bytes() const;

    const std::vector<Entry>& Entries() const { return m_entries; }
    // The key's rectangle (any case), or nullptr.
    const TetRect* Find(const std::string& key) const;
    // Changes a key's rectangle; false when there is no such key.
    bool Set(const std::string& key, const TetRect& rect);
    // Adds a key (made uppercase) with a rectangle; false with `error` when it is there already or not a valid name.
    bool Add(const std::string& key, const TetRect& rect, std::string& error);

private:
    std::vector<Entry> m_entries;   // sorted by key
};

// The .tet a texture's UV rectangles come from, as the client names it: the texture's name with its last three
// characters replaced by "TET".
std::string TetName(const std::string& texture);

// A key name a new key may have: letters, digits and underscores (made uppercase).
bool ValidKeyName(const std::string& key);
std::string UpperKey(const std::string& key);

// Where the scripts under the game folder's data/ (every .lua) name a texture's key: the lines that have both in
// quotes, without case. Code (C++) that names it is not seen.
struct KeyUse {
    std::filesystem::path script;   // relative to data/
    int line = 0;
};
std::vector<KeyUse> FindKeyUses(const std::filesystem::path& gameFolder, const std::string& texture,
                                const std::string& key);

}  // namespace x2::ui_editor
