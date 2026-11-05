// The XML header of .kom archives. The original parsed it with libxml2; the format is a flat list of elements, so a
// small strict parser is enough and keeps libxml2 out of the client's file loading.
#include "codec_internal.h"

#include <charconv>
#include <cstdio>

namespace x2::kom {

namespace {

class XmlReader
{
public:
    explicit XmlReader(std::string_view s) : s_(s) {}

    bool AtEnd() const { return p_ >= s_.size(); }
    std::size_t Position() const { return p_; }

    void SkipSpace()
    {
        while (p_ < s_.size() && (s_[p_] == ' ' || s_[p_] == '\t' || s_[p_] == '\r' || s_[p_] == '\n'))
            ++p_;
    }

    bool Consume(std::string_view token)
    {
        if (s_.substr(p_, token.size()) != token)
            return false;
        p_ += token.size();
        return true;
    }

    // Skips everything up to and including `token`.
    bool SkipPast(std::string_view token)
    {
        const std::size_t at = s_.find(token, p_);
        if (at == std::string_view::npos)
            return false;
        p_ = at + token.size();
        return true;
    }

    // Skips the XML declaration and comments before the root element.
    bool SkipProlog(std::string& error)
    {
        Consume("\xEF\xBB\xBF");
        for (;;)
        {
            SkipSpace();
            if (Consume("<?"))
            {
                if (!SkipPast("?>"))
                    return Fail(error, "unterminated XML declaration");
            }
            else if (Consume("<!--"))
            {
                if (!SkipPast("-->"))
                    return Fail(error, "unterminated comment");
            }
            else
            {
                return true;
            }
        }
    }

    // Reads a name made of letters, digits, '_', '-', '.', ':'.
    std::string_view Name()
    {
        const std::size_t start = p_;
        while (p_ < s_.size())
        {
            const char c = s_[p_];
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '-' ||
                c == '.' || c == ':')
                ++p_;
            else
                break;
        }
        return s_.substr(start, p_ - start);
    }

    // Reads attributes up to '>' or '/>'. `selfClosed` tells which one ended the tag.
    template <typename OnAttribute>
    bool Attributes(bool& selfClosed, std::string& error, OnAttribute&& onAttribute)
    {
        for (;;)
        {
            const std::size_t before = p_;
            SkipSpace();
            if (Consume("/>"))
            {
                selfClosed = true;
                return true;
            }
            if (Consume(">"))
            {
                selfClosed = false;
                return true;
            }
            if (p_ == before)
                return Fail(error, "expected a space before an attribute at byte " + std::to_string(p_));
            const std::string_view name = Name();
            if (name.empty())
                return Fail(error, "bad attribute");
            SkipSpace();
            if (!Consume("="))
                return Fail(error, "missing '=' after attribute " + std::string(name));
            SkipSpace();
            if (AtEnd() || (s_[p_] != '"' && s_[p_] != '\''))
                return Fail(error, "unquoted attribute " + std::string(name));
            const char quote = s_[p_++];
            const std::size_t end = s_.find(quote, p_);
            if (end == std::string_view::npos)
                return Fail(error, "unterminated attribute " + std::string(name));
            std::string value;
            if (!Unescape(s_.substr(p_, end - p_), value))
                return Fail(error, "bad character reference in attribute " + std::string(name));
            p_ = end + 1;
            if (!onAttribute(name, value, error))
                return false;
        }
    }

    static bool Fail(std::string& error, std::string message)
    {
        error = std::move(message);
        return false;
    }

private:
    static void AppendUtf8(std::string& out, std::uint32_t cp)
    {
        if (cp < 0x80)
            out += static_cast<char>(cp);
        else if (cp < 0x800)
        {
            out += static_cast<char>(0xC0 | (cp >> 6));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        }
        else if (cp < 0x10000)
        {
            out += static_cast<char>(0xE0 | (cp >> 12));
            out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        }
        else
        {
            out += static_cast<char>(0xF0 | (cp >> 18));
            out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        }
    }

    static bool Unescape(std::string_view in, std::string& out)
    {
        for (std::size_t i = 0; i < in.size(); ++i)
        {
            if (in[i] == '<')
                return false;
            if (in[i] != '&')
            {
                out += in[i];
                continue;
            }
            const std::size_t end = in.find(';', i);
            if (end == std::string_view::npos)
                return false;
            const std::string_view ref = in.substr(i + 1, end - i - 1);
            if (ref == "amp")
                out += '&';
            else if (ref == "lt")
                out += '<';
            else if (ref == "gt")
                out += '>';
            else if (ref == "quot")
                out += '"';
            else if (ref == "apos")
                out += '\'';
            else if (ref.size() > 1 && ref[0] == '#')
            {
                const bool hex = ref[1] == 'x' || ref[1] == 'X';
                const std::string_view digits = ref.substr(hex ? 2 : 1);
                std::uint32_t cp = 0;
                const auto [ptr, ec] = std::from_chars(digits.data(), digits.data() + digits.size(), cp, hex ? 16 : 10);
                if (ec != std::errc() || ptr != digits.data() + digits.size() || cp == 0 || cp > 0x10FFFF ||
                    (cp >= 0xD800 && cp <= 0xDFFF))
                    return false;
                AppendUtf8(out, cp);
            }
            else
                return false;
            i = end;
        }
        return true;
    }

    std::string_view s_;
    std::size_t p_ = 0;
};

bool ParseU32(std::string_view text, int base, std::uint32_t& value)
{
    if (text.empty())
        return false;
    const auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), value, base);
    return ec == std::errc() && ptr == text.data() + text.size();
}

} // namespace

bool ParseXmlHeader(std::string_view xml, std::vector<Entry>& entries, std::string& error)
{
    entries.clear();
    // Some packers pad the header with NULs.
    while (!xml.empty() && xml.back() == '\0')
        xml.remove_suffix(1);

    XmlReader r(xml);
    if (!r.SkipProlog(error))
        return false;
    if (!r.Consume("<Files"))
        return XmlReader::Fail(error, "the root element is not <Files>");
    bool selfClosed = false;
    if (!r.Attributes(selfClosed, error, [](std::string_view, const std::string&, std::string&) { return true; }))
        return false;

    while (!selfClosed)
    {
        r.SkipSpace();
        if (r.Consume("</Files>"))
            break;
        if (r.Consume("<!--"))
        {
            if (!r.SkipPast("-->"))
                return XmlReader::Fail(error, "unterminated comment");
            continue;
        }
        if (!r.Consume("<File") || r.AtEnd())
            return XmlReader::Fail(error, "expected <File> at byte " + std::to_string(r.Position()));

        Entry e;
        bool hasName = false, hasSize = false, hasStored = false;
        bool fileClosed = false;
        const bool ok = r.Attributes(fileClosed, error, [&](std::string_view name, const std::string& value,
                                                             std::string& err) {
            bool good = true;
            if (name == "Name")
            {
                e.name = value;
                hasName = true;
            }
            else if (name == "Size")
                good = hasSize = ParseU32(value, 10, e.size);
            else if (name == "CompressedSize")
                good = hasStored = ParseU32(value, 10, e.storedSize);
            else if (name == "Checksum")
                good = ParseU32(value, 16, e.checksum);
            else if (name == "FileTime")
                good = ParseU32(value, 16, e.fileTime);
            else if (name == "Algorithm")
                good = ParseU32(value, 10, e.algorithm);
            if (!good)
                err = "bad " + std::string(name) + " value \"" + value + "\"";
            return good;
        });
        if (!ok)
            return false;
        if (!fileClosed)
        {
            r.SkipSpace();
            if (!r.Consume("</File>"))
                return XmlReader::Fail(error, "unterminated <File>");
        }
        if (!hasName || e.name.empty() || !hasSize || !hasStored)
            return XmlReader::Fail(error, "<File> " + std::to_string(entries.size()) +
                                              " lacks Name, Size or CompressedSize");
        entries.push_back(std::move(e));
    }

    r.SkipSpace();
    if (!r.AtEnd())
        return XmlReader::Fail(error, "unexpected data after </Files>");
    return true;
}

namespace {

void AppendEscaped(std::string& out, std::string_view s)
{
    for (const char c : s)
    {
        switch (c)
        {
        case '&': out += "&amp;"; break;
        case '<': out += "&lt;"; break;
        case '>': out += "&gt;"; break;
        case '"': out += "&quot;"; break;
        default: out += c; break;
        }
    }
}

} // namespace

std::string GenerateXmlHeader(const std::vector<Entry>& entries)
{
    std::string xml = "<?xml version=\"1.0\"?><Files>";
    char numbers[128];
    for (const Entry& e : entries)
    {
        xml += "<File Name=\"";
        AppendEscaped(xml, e.name);
        std::snprintf(numbers, sizeof numbers,
                      "\" Size=\"%u\" CompressedSize=\"%u\" Checksum=\"%08x\" FileTime=\"%08x\" Algorithm=\"%u\" />",
                      e.size, e.storedSize, e.checksum, e.fileTime, e.algorithm);
        xml += numbers;
    }
    xml += "</Files>";
    return xml;
}

} // namespace x2::kom
