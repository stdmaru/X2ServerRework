// DirectX .x files: header, MSZIP decompression, and the text and binary token streams turned into data objects.
// docs/porting/client-d3d9.md#x-files-2026-11-07-clientx-loader

#include "XFile.h"

#include <zlib.h>

#include <charconv>
#include <cstring>

namespace x2::d3d9::xfile {
namespace {

constexpr std::size_t kHeaderSize = 16;

unsigned U8(std::span<const std::byte> bytes, std::size_t at) { return std::to_integer<unsigned>(bytes[at]); }

std::uint32_t U16(std::span<const std::byte> bytes, std::size_t at)
{
    return U8(bytes, at) | (U8(bytes, at + 1) << 8);
}

std::uint32_t U32(std::span<const std::byte> bytes, std::size_t at)
{
    return U16(bytes, at) | (U16(bytes, at + 2) << 16);
}

bool Is(std::span<const std::byte> bytes, std::size_t at, const char* text)
{
    const std::size_t length = std::strlen(text);
    if (at + length > bytes.size()) {
        return false;
    }
    return std::memcmp(bytes.data() + at, text, length) == 0;
}

// --- MSZIP ----------------------------------------------------------------------------------------------------------

// One block's raw deflate data inflated into `out` (`size` bytes) with `dictionary`. False unless it gives exactly `size`
// bytes.
bool Inflate(std::span<const std::byte> in, std::span<const std::byte> dictionary, std::byte* out, std::size_t size)
{
    z_stream z{};
    if (inflateInit2(&z, -MAX_WBITS) != Z_OK) {
        return false;
    }
    if (!dictionary.empty()) {
        inflateSetDictionary(&z, reinterpret_cast<const Bytef*>(dictionary.data()), static_cast<uInt>(dictionary.size()));
    }
    z.next_in = reinterpret_cast<Bytef*>(const_cast<std::byte*>(in.data()));
    z.avail_in = static_cast<uInt>(in.size());
    z.next_out = reinterpret_cast<Bytef*>(out);
    z.avail_out = static_cast<uInt>(size);
    const int result = inflate(&z, Z_FINISH);
    const std::size_t produced = z.total_out;
    inflateEnd(&z);
    return (result == Z_STREAM_END || result == Z_OK || result == Z_BUF_ERROR) && produced == size;
}

// After the header: the uncompressed size (header included), then blocks of {uncompressed size, compressed size,
// "CK", deflate data}; each block's dictionary is the 32 KB of output before it.
//
// Some blocks (49 of the client's files, 2026-11-07) end in a stored deflate block written with its length but without
// the length's complement (NLEN), which zlib rejects: those are inflated again with the complement put back.
bool Decompress(std::span<const std::byte> file, std::vector<std::byte>& out, std::string& error)
{
    if (file.size() < kHeaderSize + 4) {
        error = "compressed .x: no size";
        return false;
    }
    const std::uint32_t total = U32(file, kHeaderSize);
    if (total < kHeaderSize || total > (1u << 30)) {
        error = "compressed .x: bad size";
        return false;
    }
    out.assign(file.begin(), file.begin() + kHeaderSize);
    out.reserve(total);
    std::size_t at = kHeaderSize + 4;
    std::vector<std::byte> patched;
    while (out.size() < total && at + 4 <= file.size()) {
        const std::uint32_t plain = U16(file, at), packed = U16(file, at + 2);
        at += 4;
        if (packed < 2 || at + packed > file.size() || !Is(file, at, "CK")) {
            error = "compressed .x: bad block";
            return false;
        }
        const std::size_t start = out.size();
        const std::size_t size = plain == 0 ? 32768 : plain;
        const std::span<const std::byte> data = file.subspan(at + 2, packed - 2);
        const std::size_t dictionary = std::min<std::size_t>(start - kHeaderSize, 32768);
        out.resize(start + size);
        auto inflateInto = [&](std::span<const std::byte> in) {
            return Inflate(in, std::span<const std::byte>(out.data() + start - dictionary, dictionary),
                           out.data() + start, size);
        };
        bool ok = inflateInto(data);
        // The stored tail: its `length` last bytes, the length just before them.
        for (std::size_t length = 1; !ok && length + 2 <= data.size() && length <= size; ++length) {
            if (U16(data, data.size() - length - 2) != length) {
                continue;
            }
            const std::size_t tail = data.size() - length;
            patched.assign(data.begin(), data.begin() + static_cast<std::ptrdiff_t>(tail));
            patched.push_back(static_cast<std::byte>(~length & 0xFF));
            patched.push_back(static_cast<std::byte>((~length >> 8) & 0xFF));
            patched.insert(patched.end(), data.begin() + static_cast<std::ptrdiff_t>(tail), data.end());
            ok = inflateInto(patched);
        }
        if (!ok) {
            error = "compressed .x: bad deflate data";
            return false;
        }
        at += packed;
    }
    if (out.size() != total) {
        error = "compressed .x: " + std::to_string(out.size()) + " of " + std::to_string(total) + " bytes";
        return false;
    }
    return true;
}

// --- Text -----------------------------------------------------------------------------------------------------------

class TextReader {
public:
    explicit TextReader(std::span<const std::byte> bytes) : m_text(reinterpret_cast<const char*>(bytes.data()), bytes.size()) {}

    bool Read(File& file, std::string& error)
    {
        m_at = kHeaderSize;
        for (;;) {
            Token t = Next();
            if (t.kind == Kind::End) {
                return true;
            }
            if (t.kind == Kind::Word && t.text == "template") {
                if (!SkipTemplate()) {
                    error = "text .x: unterminated template";
                    return false;
                }
                continue;
            }
            if (t.kind == Kind::Open) {
                Object reference;
                if (!ReadReference(reference)) {
                    error = "text .x: bad reference";
                    return false;
                }
                file.objects.push_back(std::move(reference));
                continue;
            }
            if (t.kind != Kind::Word) {
                error = "text .x: unexpected '" + std::string(t.text) + "' at top level";
                return false;
            }
            Object object;
            if (!ReadObject(std::string(t.text), object, error)) {
                return false;
            }
            file.objects.push_back(std::move(object));
        }
    }

private:
    enum class Kind { End, Word, Number, String, Open, Close, Guid, Separator };
    struct Token {
        Kind kind = Kind::End;
        std::string_view text;
    };

    static bool IsWordChar(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-' ||
               c == '.' || c == '+' || static_cast<unsigned char>(c) >= 0x80;
    }

    Token Next()
    {
        for (;;) {
            while (m_at < m_text.size() && static_cast<unsigned char>(m_text[m_at]) <= ' ') {
                ++m_at;
            }
            if (m_at >= m_text.size()) {
                return {};
            }
            const char c = m_text[m_at];
            if (c == '#' || (c == '/' && m_at + 1 < m_text.size() && m_text[m_at + 1] == '/')) {
                while (m_at < m_text.size() && m_text[m_at] != '\n') {
                    ++m_at;
                }
                continue;
            }
            const std::size_t start = m_at;
            switch (c) {
            case '{': ++m_at; return {Kind::Open, m_text.substr(start, 1)};
            case '}': ++m_at; return {Kind::Close, m_text.substr(start, 1)};
            case ',':
            case ';': ++m_at; return {Kind::Separator, m_text.substr(start, 1)};
            case '<': {
                const std::size_t end = m_text.find('>', m_at);
                m_at = end == std::string_view::npos ? m_text.size() : end + 1;
                return {Kind::Guid, m_text.substr(start, m_at - start)};
            }
            case '"': {
                const std::size_t end = m_text.find('"', m_at + 1);
                m_at = end == std::string_view::npos ? m_text.size() : end + 1;
                return {Kind::String, m_text.substr(start + 1, (end == std::string_view::npos ? m_text.size() : end) - start - 1)};
            }
            default: break;
            }
            while (m_at < m_text.size() && IsWordChar(m_text[m_at])) {
                ++m_at;
            }
            if (m_at == start) {
                ++m_at;     // a character no token has: skipped
                continue;
            }
            const std::string_view word = m_text.substr(start, m_at - start);
            const bool number = (c >= '0' && c <= '9') || ((c == '-' || c == '+' || c == '.') && word.size() > 1 &&
                                                            ((word[1] >= '0' && word[1] <= '9') || word[1] == '.'));
            return {number ? Kind::Number : Kind::Word, word};
        }
    }

    bool SkipTemplate()
    {
        int depth = 0;
        for (Token t = Next(); t.kind != Kind::End; t = Next()) {
            if (t.kind == Kind::Open) {
                ++depth;
            } else if (t.kind == Kind::Close && --depth == 0) {
                return true;
            }
        }
        return false;
    }

    // After '{': a name (and maybe a GUID) and '}'.
    bool ReadReference(Object& reference)
    {
        for (Token t = Next(); t.kind != Kind::End; t = Next()) {
            if (t.kind == Kind::Close) {
                return !reference.name.empty();
            }
            if (t.kind == Kind::Word || t.kind == Kind::Number) {
                reference.name = std::string(t.text);
            }
        }
        return false;
    }

    bool ReadObject(std::string type, Object& object, std::string& error)
    {
        object.type = std::move(type);
        Token t = Next();
        if (t.kind == Kind::Word || t.kind == Kind::Number) {
            object.name = std::string(t.text);
            t = Next();
        }
        if (t.kind != Kind::Open) {
            error = "text .x: '{' expected after " + object.type;
            return false;
        }
        for (t = Next();; t = Next()) {
            switch (t.kind) {
            case Kind::End: error = "text .x: unterminated " + object.type; return false;
            case Kind::Close: return true;
            case Kind::Separator:
            case Kind::Guid: break;
            case Kind::Number: {
                double value = 0;
                const auto result = std::from_chars(t.text.data() + (t.text[0] == '+' ? 1 : 0),
                                                    t.text.data() + t.text.size(), value);
                if (result.ec != std::errc()) {
                    value = 0;      // "-1.#IND00" and the like
                }
                object.data.push_back({value, -1});
                break;
            }
            case Kind::String:
                object.data.push_back({0, static_cast<std::int32_t>(object.strings.size())});
                object.strings.emplace_back(t.text);
                break;
            case Kind::Open: {
                Object reference;
                if (!ReadReference(reference)) {
                    error = "text .x: bad reference in " + object.type;
                    return false;
                }
                object.children.push_back(std::move(reference));
                break;
            }
            case Kind::Word: {
                Object child;
                if (!ReadObject(std::string(t.text), child, error)) {
                    return false;
                }
                object.children.push_back(std::move(child));
                break;
            }
            }
        }
    }

    std::string_view m_text;
    std::size_t m_at = 0;
};

// --- Binary ---------------------------------------------------------------------------------------------------------

enum BinaryToken : std::uint32_t {
    kName = 1, kString = 2, kInteger = 3, kGuid = 5, kIntegerList = 6, kFloatList = 7,
    kOpenBrace = 10, kCloseBrace = 11, kTemplate = 31,
};

class BinaryReader {
public:
    BinaryReader(std::span<const std::byte> bytes, int floatBits) : m_bytes(bytes), m_floatBytes(floatBits / 8) {}

    bool Read(File& file, std::string& error)
    {
        m_at = kHeaderSize;
        while (m_at < m_bytes.size()) {
            const std::optional<std::uint32_t> token = Token();
            if (!token) {
                break;      // trailing bytes shorter than a token
            }
            if (*token == kTemplate) {
                if (!SkipTemplate()) {
                    return Fail(error, "unterminated template");
                }
            } else if (*token == kName) {
                Object object;
                if (!ReadObject(object, error)) {
                    return false;
                }
                file.objects.push_back(std::move(object));
            } else if (*token == kOpenBrace) {
                Object reference;
                if (!ReadReference(reference)) {
                    return Fail(error, "bad reference");
                }
                file.objects.push_back(std::move(reference));
            } else if (!SkipPayload(*token)) {
                return Fail(error, "bad token " + std::to_string(*token) + " at top level");
            }
        }
        return true;
    }

private:
    bool Fail(std::string& error, const std::string& what)
    {
        error = "binary .x: " + what + " (offset " + std::to_string(m_at) + ")";
        return false;
    }

    bool Has(std::size_t bytes) const { return m_at + bytes <= m_bytes.size(); }

    std::optional<std::uint32_t> Token()
    {
        if (!Has(2)) {
            return std::nullopt;
        }
        const std::uint32_t token = U16(m_bytes, m_at);
        m_at += 2;
        return token;
    }

    std::optional<std::uint32_t> Dword()
    {
        if (!Has(4)) {
            return std::nullopt;
        }
        const std::uint32_t value = U32(m_bytes, m_at);
        m_at += 4;
        return value;
    }

    std::optional<std::string> Text()
    {
        const std::optional<std::uint32_t> length = Dword();
        if (!length || !Has(*length)) {
            return std::nullopt;
        }
        std::string text(reinterpret_cast<const char*>(m_bytes.data() + m_at), *length);
        m_at += *length;
        return text;
    }

    // The payload of a token that is neither a name nor a brace (those are handled by the callers).
    bool SkipPayload(std::uint32_t token)
    {
        switch (token) {
        case kString: return Text().has_value() && Token().has_value();
        case kInteger: return Dword().has_value();
        case kGuid:
            if (!Has(16)) {
                return false;
            }
            m_at += 16;
            return true;
        case kIntegerList:
        case kFloatList: {
            const std::optional<std::uint32_t> count = Dword();
            const std::size_t size = token == kIntegerList ? 4 : static_cast<std::size_t>(m_floatBytes);
            if (!count || !Has(static_cast<std::size_t>(*count) * size)) {
                return false;
            }
            m_at += static_cast<std::size_t>(*count) * size;
            return true;
        }
        default: return token >= 10 && token <= 53;      // punctuation and template keywords have no payload
        }
    }

    bool SkipTemplate()
    {
        int depth = 0;
        for (std::optional<std::uint32_t> t = Token(); t; t = Token()) {
            if (*t == kName) {
                if (!Text()) {
                    return false;
                }
            } else if (*t == kOpenBrace) {
                ++depth;
            } else if (*t == kCloseBrace) {
                if (--depth == 0) {
                    return true;
                }
            } else if (!SkipPayload(*t)) {
                return false;
            }
        }
        return false;
    }

    bool ReadReference(Object& reference)
    {
        for (std::optional<std::uint32_t> t = Token(); t; t = Token()) {
            if (*t == kCloseBrace) {
                return !reference.name.empty();
            }
            if (*t == kName) {
                const std::optional<std::string> name = Text();
                if (!name) {
                    return false;
                }
                reference.name = *name;
            } else if (!SkipPayload(*t)) {
                return false;
            }
        }
        return false;
    }

    // After the type's name token.
    bool ReadObject(Object& object, std::string& error)
    {
        std::optional<std::string> type = Text();
        if (!type) {
            return Fail(error, "bad name");
        }
        object.type = std::move(*type);
        // An optional name and GUID, then '{'.
        for (;;) {
            const std::optional<std::uint32_t> t = Token();
            if (!t) {
                return Fail(error, "'{' expected after " + object.type);
            }
            if (*t == kOpenBrace) {
                break;
            }
            if (*t == kName) {
                std::optional<std::string> name = Text();
                if (!name) {
                    return Fail(error, "bad name");
                }
                object.name = std::move(*name);
            } else if (*t != kGuid || !SkipPayload(*t)) {
                return Fail(error, "'{' expected after " + object.type);
            }
        }
        for (;;) {
            const std::optional<std::uint32_t> t = Token();
            if (!t) {
                return Fail(error, "unterminated " + object.type);
            }
            switch (*t) {
            case kCloseBrace: return true;
            case kName: {
                Object child;
                if (!ReadObject(child, error)) {
                    return false;
                }
                object.children.push_back(std::move(child));
                break;
            }
            case kOpenBrace: {
                Object reference;
                if (!ReadReference(reference)) {
                    return Fail(error, "bad reference in " + object.type);
                }
                object.children.push_back(std::move(reference));
                break;
            }
            case kString: {
                std::optional<std::string> text = Text();
                if (!text || !Token()) {
                    return Fail(error, "bad string");
                }
                object.data.push_back({0, static_cast<std::int32_t>(object.strings.size())});
                object.strings.push_back(std::move(*text));
                break;
            }
            case kInteger: {
                const std::optional<std::uint32_t> value = Dword();
                if (!value) {
                    return Fail(error, "bad integer");
                }
                object.data.push_back({static_cast<double>(*value), -1});
                break;
            }
            case kIntegerList:
            case kFloatList: {
                const std::optional<std::uint32_t> count = Dword();
                const std::size_t size = *t == kIntegerList ? 4 : static_cast<std::size_t>(m_floatBytes);
                if (!count || !Has(static_cast<std::size_t>(*count) * size)) {
                    return Fail(error, "bad list");
                }
                object.data.reserve(object.data.size() + *count);
                for (std::uint32_t i = 0; i < *count; ++i) {
                    double value = 0;
                    if (*t == kIntegerList) {
                        value = U32(m_bytes, m_at);
                    } else if (m_floatBytes == 4) {
                        const std::uint32_t bits = U32(m_bytes, m_at);
                        float f;
                        std::memcpy(&f, &bits, 4);
                        value = f;
                    } else {
                        const std::uint64_t bits = U32(m_bytes, m_at) |
                                                   (static_cast<std::uint64_t>(U32(m_bytes, m_at + 4)) << 32);
                        std::memcpy(&value, &bits, 8);
                    }
                    object.data.push_back({value, -1});
                    m_at += size;
                }
                break;
            }
            default:
                if (!SkipPayload(*t)) {
                    return Fail(error, "bad token " + std::to_string(*t) + " in " + object.type);
                }
                break;
            }
        }
    }

    std::span<const std::byte> m_bytes;
    int m_floatBytes = 4;
    std::size_t m_at = 0;
};

}  // namespace

std::optional<Header> ReadHeader(std::span<const std::byte> file)
{
    if (file.size() < kHeaderSize || !Is(file, 0, "xof ")) {
        return std::nullopt;
    }
    Header header;
    if (Is(file, 8, "txt ")) {
        header.encoding = Encoding::Text;
    } else if (Is(file, 8, "bin ")) {
        header.encoding = Encoding::Binary;
    } else if (Is(file, 8, "tzip")) {
        header.encoding = Encoding::Text;
        header.compressed = true;
    } else if (Is(file, 8, "bzip")) {
        header.encoding = Encoding::Binary;
        header.compressed = true;
    } else {
        return std::nullopt;
    }
    if (Is(file, 12, "0032")) {
        header.floatBits = 32;
    } else if (Is(file, 12, "0064")) {
        header.floatBits = 64;
    } else {
        return std::nullopt;
    }
    return header;
}

std::optional<File> Read(std::span<const std::byte> bytes, std::string& error)
{
    const std::optional<Header> header = ReadHeader(bytes);
    if (!header) {
        error = "not a .x file";
        return std::nullopt;
    }
    std::vector<std::byte> plain;
    if (header->compressed) {
        if (!Decompress(bytes, plain, error)) {
            return std::nullopt;
        }
        bytes = plain;
    }
    File file;
    const bool ok = header->encoding == Encoding::Text ? TextReader(bytes).Read(file, error)
                                                       : BinaryReader(bytes, header->floatBits).Read(file, error);
    if (!ok) {
        return std::nullopt;
    }
    return file;
}

double Cursor::Number()
{
    while (m_next < m_object.data.size()) {
        const Datum& d = m_object.data[m_next++];
        if (d.string < 0) {
            return d.number;
        }
    }
    m_failed = true;
    return 0;
}

std::uint32_t Cursor::Dword()
{
    const double value = Number();
    return value >= 0 && value <= 4294967295.0 ? static_cast<std::uint32_t>(value) : 0;
}

std::string Cursor::String()
{
    while (m_next < m_object.data.size()) {
        const Datum& d = m_object.data[m_next++];
        if (d.string >= 0) {
            return m_object.strings[static_cast<std::size_t>(d.string)];
        }
    }
    m_failed = true;
    return {};
}

std::vector<const Object*> ChildrenOfType(const Object& object, const char* type)
{
    std::vector<const Object*> out;
    for (const Object& child : object.children) {
        if (child.type == type) {
            out.push_back(&child);
        }
    }
    return out;
}

const Object* FirstChildOfType(const Object& object, const char* type)
{
    for (const Object& child : object.children) {
        if (child.type == type) {
            return &child;
        }
    }
    return nullptr;
}

}  // namespace x2::d3d9::xfile
