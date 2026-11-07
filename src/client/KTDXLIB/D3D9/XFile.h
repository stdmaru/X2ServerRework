#pragma once
// DirectX .x files as a tree of data objects: text, binary, and their MSZIP-compressed forms (tzip, bzip), with 32- and
// 64-bit floats. Template definitions are skipped: the loaders know the templates they read (decision T37: the assets
// load as they are, through our own parser). docs/porting/client-d3d9.md#x-files-2026-11-07-clientx-loader

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace x2::d3d9::xfile {

// A data member: a number (integers and floats alike) or a string.
struct Datum {
    double number = 0;
    std::int32_t string = -1;       // index into Object::strings, or -1 for a number
};

struct Object {
    std::string type;               // the template's name ("Mesh", "Frame"...); empty for a reference
    std::string name;               // the object's name, or the name a reference points to
    std::vector<Datum> data;
    std::vector<std::string> strings;
    std::vector<Object> children;   // data objects and references, in file order

    bool IsReference() const { return type.empty(); }
};

struct File {
    std::vector<Object> objects;    // top-level data objects and references
};

enum class Encoding { Text, Binary };

struct Header {
    Encoding encoding = Encoding::Text;
    bool compressed = false;        // tzip/bzip
    int floatBits = 32;
};

// The 16-byte header ("xof 0303bzip0032"). Nothing for a file that is not a .x.
std::optional<Header> ReadHeader(std::span<const std::byte> file);
// The whole file. Nothing (and `error`) for a file that cannot be read.
std::optional<File> Read(std::span<const std::byte> file, std::string& error);

// Reads an object's data members in order. Reading past the end gives 0 / an empty string and sets Failed().
class Cursor {
public:
    explicit Cursor(const Object& object) : m_object(object) {}
    double Number();
    float Float() { return static_cast<float>(Number()); }
    std::uint32_t Dword();
    std::string String();
    bool Failed() const { return m_failed; }
    std::size_t Remaining() const { return m_object.data.size() - m_next; }

private:
    const Object& m_object;
    std::size_t m_next = 0;
    bool m_failed = false;
};

// Children of a type (references among the children are not followed).
std::vector<const Object*> ChildrenOfType(const Object& object, const char* type);
const Object* FirstChildOfType(const Object& object, const char* type);

}  // namespace x2::d3d9::xfile
