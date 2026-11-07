#include "x2/platform/xml.h"

#include <cstdint>
#include <cstdlib>

namespace x2::platform {

const std::string* XmlElement::Attribute(std::string_view attributeName) const
{
    for (const auto& [key, value] : attributes) {
        if (key == attributeName)
            return &value;
    }
    return nullptr;
}

const XmlElement* XmlElement::Child(std::string_view childName) const
{
    for (const XmlElement& child : children) {
        if (child.name == childName)
            return &child;
    }
    return nullptr;
}

namespace {

class Parser
{
public:
    explicit Parser(std::string_view text) : m_text(text) {}

    std::optional<XmlElement> Document()
    {
        if (m_text.substr(0, 3) == "\xEF\xBB\xBF")
            m_pos = 3;
        if (!Misc())
            return std::nullopt;
        if (!Peek('<'))
            return Fail("expected the root element");
        XmlElement root;
        if (!Element(root) || !Misc())
            return std::nullopt;
        if (m_pos != m_text.size())
            return Fail("content after the root element");
        return root;
    }

    std::string error;

private:
    std::nullopt_t Fail(const char* message)
    {
        if (error.empty())
            error = std::string(message) + " at byte " + std::to_string(m_pos);
        return std::nullopt;
    }

    bool Peek(char c) const { return m_pos < m_text.size() && m_text[m_pos] == c; }
    bool StartsWith(std::string_view s) const { return m_text.substr(m_pos, s.size()) == s; }
    static bool IsSpace(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
    static bool IsNameChar(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == ':' ||
               c == '-' || c == '.' || static_cast<unsigned char>(c) >= 0x80;
    }
    void SkipSpace()
    {
        while (m_pos < m_text.size() && IsSpace(m_text[m_pos]))
            ++m_pos;
    }

    // Skips past `end`; false at the end of the text.
    bool SkipPast(std::string_view end)
    {
        const std::size_t at = m_text.find(end, m_pos);
        if (at == std::string_view::npos)
            return false;
        m_pos = at + end.size();
        return true;
    }

    // Spaces, comments, processing instructions and a DOCTYPE, outside the root element.
    bool Misc()
    {
        for (;;) {
            SkipSpace();
            if (StartsWith("<?")) {
                if (!SkipPast("?>"))
                    return Fail("unterminated processing instruction"), false;
            } else if (StartsWith("<!--")) {
                if (!SkipPast("-->"))
                    return Fail("unterminated comment"), false;
            } else if (StartsWith("<!DOCTYPE")) {
                if (!SkipDoctype())
                    return Fail("unterminated DOCTYPE"), false;
            } else {
                return true;
            }
        }
    }

    bool SkipDoctype()
    {
        int depth = 0;
        for (; m_pos < m_text.size(); ++m_pos) {
            const char c = m_text[m_pos];
            if (c == '[')
                ++depth;
            else if (c == ']')
                --depth;
            else if (c == '>' && depth == 0) {
                ++m_pos;
                return true;
            }
        }
        return false;
    }

    bool Name(std::string& out)
    {
        const std::size_t start = m_pos;
        while (m_pos < m_text.size() && IsNameChar(m_text[m_pos]))
            ++m_pos;
        if (m_pos == start)
            return Fail("expected a name"), false;
        out.assign(m_text.substr(start, m_pos - start));
        return true;
    }

    static void AppendUtf8(std::string& out, std::uint32_t code)
    {
        if (code < 0x80) {
            out += static_cast<char>(code);
        } else if (code < 0x800) {
            out += static_cast<char>(0xC0 | (code >> 6));
            out += static_cast<char>(0x80 | (code & 0x3F));
        } else if (code < 0x10000) {
            out += static_cast<char>(0xE0 | (code >> 12));
            out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (code & 0x3F));
        } else {
            out += static_cast<char>(0xF0 | (code >> 18));
            out += static_cast<char>(0x80 | ((code >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (code & 0x3F));
        }
    }

    // Text up to `stop` (exclusive), with references decoded.
    bool Chars(std::string& out, char stop)
    {
        while (m_pos < m_text.size() && m_text[m_pos] != stop) {
            const char c = m_text[m_pos];
            if (c == '<' && stop != '<')
                return Fail("'<' in an attribute value"), false;
            if (c != '&') {
                out += c;
                ++m_pos;
                continue;
            }
            const std::size_t end = m_text.find(';', m_pos);
            if (end == std::string_view::npos)
                return Fail("unterminated reference"), false;
            const std::string_view ref = m_text.substr(m_pos + 1, end - m_pos - 1);
            if (ref == "lt") out += '<';
            else if (ref == "gt") out += '>';
            else if (ref == "amp") out += '&';
            else if (ref == "apos") out += '\'';
            else if (ref == "quot") out += '"';
            else if (ref.size() > 1 && ref[0] == '#') {
                const bool hex = ref[1] == 'x';
                const std::string digits(ref.substr(hex ? 2 : 1));
                char* parsedEnd = nullptr;
                const unsigned long code = std::strtoul(digits.c_str(), &parsedEnd, hex ? 16 : 10);
                if (digits.empty() || *parsedEnd != '\0' || code > 0x10FFFF)
                    return Fail("bad character reference"), false;
                AppendUtf8(out, static_cast<std::uint32_t>(code));
            } else {
                return Fail("unknown entity"), false;
            }
            m_pos = end + 1;
        }
        return true;
    }

    bool Element(XmlElement& element)
    {
        ++m_pos;   // '<'
        if (!Name(element.name))
            return false;
        for (;;) {
            const std::size_t before = m_pos;
            SkipSpace();
            if (StartsWith("/>")) {
                m_pos += 2;
                return true;
            }
            if (Peek('>')) {
                ++m_pos;
                break;
            }
            if (m_pos == before)
                return Fail("expected an attribute"), false;
            std::string key, value;
            if (!Name(key))
                return false;
            SkipSpace();
            if (!Peek('='))
                return Fail("expected '='"), false;
            ++m_pos;
            SkipSpace();
            if (!Peek('"') && !Peek('\''))
                return Fail("expected a quoted value"), false;
            const char quote = m_text[m_pos++];
            if (!Chars(value, quote))
                return false;
            if (!Peek(quote))
                return Fail("unterminated attribute value"), false;
            ++m_pos;
            element.attributes.emplace_back(std::move(key), std::move(value));
        }
        // Content.
        for (;;) {
            if (m_pos >= m_text.size())
                return Fail("unterminated element"), false;
            if (StartsWith("</")) {
                m_pos += 2;
                std::string closing;
                if (!Name(closing))
                    return false;
                if (closing != element.name)
                    return Fail("mismatched closing tag"), false;
                SkipSpace();
                if (!Peek('>'))
                    return Fail("expected '>'"), false;
                ++m_pos;
                return true;
            }
            if (StartsWith("<!--")) {
                if (!SkipPast("-->"))
                    return Fail("unterminated comment"), false;
            } else if (StartsWith("<![CDATA[")) {
                const std::size_t start = m_pos + 9;
                if (!SkipPast("]]>"))
                    return Fail("unterminated CDATA"), false;
                element.text.append(m_text.substr(start, m_pos - 3 - start));
            } else if (StartsWith("<?")) {
                if (!SkipPast("?>"))
                    return Fail("unterminated processing instruction"), false;
            } else if (Peek('<')) {
                XmlElement child;
                if (!Element(child))
                    return false;
                element.children.push_back(std::move(child));
            } else if (!Chars(element.text, '<')) {
                return false;
            }
        }
    }

    std::string_view m_text;
    std::size_t m_pos = 0;
};

}  // namespace

std::optional<XmlElement> ParseXml(std::string_view document, std::string* error)
{
    Parser parser(document);
    std::optional<XmlElement> root = parser.Document();
    if (!root && error != nullptr)
        *error = parser.error;
    return root;
}

}  // namespace x2::platform
