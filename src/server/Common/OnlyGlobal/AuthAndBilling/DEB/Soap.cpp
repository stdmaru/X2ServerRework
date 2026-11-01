#include "Soap.h"

#include <cstdlib>

namespace x2::deb {

namespace {

constexpr std::string_view kEnvelopeStart =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\""
    " xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\""
    " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
    " xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"";
// The gSOAP client also declared the WSDL namespace (unused in the messages).
constexpr std::string_view kRequestNamespaces =
    " xmlns:ns1=\"http://192.168.1.100:8000/AuthService.wsdl\" xmlns:ns2=\"urn:AuthService\"><SOAP-ENV:Body>";
constexpr std::string_view kResponseNamespaces = " xmlns:ns2=\"urn:AuthService\"><SOAP-ENV:Body>";
constexpr std::string_view kEnvelopeClose = "</SOAP-ENV:Body></SOAP-ENV:Envelope>";

std::string_view LocalName(std::string_view qname)
{
    const auto colon = qname.find(':');
    return colon == std::string_view::npos ? qname : qname.substr(colon + 1);
}

// Minimal tag scanner: enough for the flat messages gSOAP writes (no CDATA, no nested operations).
struct Tag
{
    std::string_view name;   // qualified name
    bool             closing = false;
    bool             empty = false;   // <a/>
    std::size_t      end = 0;         // index after '>'
};

std::optional<Tag> NextTag(std::string_view xml, std::size_t& pos)
{
    for (;;) {
        const auto lt = xml.find('<', pos);
        if (lt == std::string_view::npos)
            return std::nullopt;
        const auto gt = xml.find('>', lt);
        if (gt == std::string_view::npos)
            return std::nullopt;
        pos = gt + 1;
        std::string_view inner = xml.substr(lt + 1, gt - lt - 1);
        if (inner.empty() || inner[0] == '?' || inner[0] == '!')
            continue;  // declaration, comment
        Tag tag;
        tag.end = pos;
        if (inner[0] == '/') {
            tag.closing = true;
            inner.remove_prefix(1);
        }
        if (!inner.empty() && inner.back() == '/') {
            tag.empty = true;
            inner.remove_suffix(1);
        }
        const auto space = inner.find_first_of(" \t\r\n");
        tag.name = inner.substr(0, space);
        return tag;
    }
}

std::string XmlUnescape(std::string_view text)
{
    std::string out;
    out.reserve(text.size());
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] != '&') {
            out += text[i];
            continue;
        }
        const auto semi = text.find(';', i);
        if (semi == std::string_view::npos) {
            out += text.substr(i);
            break;
        }
        const std::string_view entity = text.substr(i + 1, semi - i - 1);
        if (entity == "lt") out += '<';
        else if (entity == "gt") out += '>';
        else if (entity == "amp") out += '&';
        else if (entity == "quot") out += '"';
        else if (entity == "apos") out += '\'';
        else if (!entity.empty() && entity[0] == '#') {
            const bool hex = entity.size() > 1 && (entity[1] == 'x' || entity[1] == 'X');
            const std::string digits(entity.substr(hex ? 2 : 1));
            const unsigned long cp = std::strtoul(digits.c_str(), nullptr, hex ? 16 : 10);
            // UTF-8 encoding of the code point
            if (cp < 0x80) out += static_cast<char>(cp);
            else if (cp < 0x800) { out += static_cast<char>(0xC0 | (cp >> 6)); out += static_cast<char>(0x80 | (cp & 0x3F)); }
            else if (cp < 0x10000) { out += static_cast<char>(0xE0 | (cp >> 12)); out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F)); out += static_cast<char>(0x80 | (cp & 0x3F)); }
            else { out += static_cast<char>(0xF0 | (cp >> 18)); out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F)); out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F)); out += static_cast<char>(0x80 | (cp & 0x3F)); }
        } else {
            out += text.substr(i, semi - i + 1);  // unknown entity: kept as written
        }
        i = semi;
    }
    return out;
}

std::string BuildMessage(std::string_view namespaces, std::string_view element, const SoapFields& fields)
{
    std::string xml(kEnvelopeStart);
    xml += namespaces;
    xml += "<ns2:";
    xml += element;
    xml += '>';
    for (const auto& [name, value] : fields) {
        xml += "<ns2:" + name + '>' + XmlEscape(value) + "</ns2:" + name + '>';
    }
    xml += "</ns2:";
    xml += element;
    xml += '>';
    xml += kEnvelopeClose;
    return xml;
}

} // namespace

const std::string& SoapMessage::Text(const char* name) const
{
    static const std::string empty;
    const auto it = fields.find(name);
    return it == fields.end() ? empty : it->second;
}

long long SoapMessage::Int(const char* name) const
{
    return std::strtoll(Text(name).c_str(), nullptr, 10);
}

std::uint64_t SoapMessage::UInt(const char* name) const
{
    return std::strtoull(Text(name).c_str(), nullptr, 10);
}

std::optional<SoapMessage> ParseSoapMessage(std::string_view xml)
{
    std::size_t pos = 0;
    // up to <...:Body>
    for (;;) {
        const auto tag = NextTag(xml, pos);
        if (!tag)
            return std::nullopt;
        if (!tag->closing && LocalName(tag->name) == "Body")
            break;
    }
    const auto op = NextTag(xml, pos);
    if (!op || op->closing)
        return std::nullopt;

    SoapMessage message;
    message.element = std::string(LocalName(op->name));
    if (op->empty)
        return message;

    for (;;) {
        const auto tag = NextTag(xml, pos);
        if (!tag)
            return std::nullopt;
        if (tag->closing)  // end of the operation element
            break;
        const std::string name(LocalName(tag->name));
        if (tag->empty) {
            message.fields[name];
            continue;
        }
        const auto valueStart = tag->end;
        const auto close = NextTag(xml, pos);
        if (!close || !close->closing || LocalName(close->name) != name)
            return std::nullopt;  // nested elements are not part of these messages
        const auto valueEnd = xml.rfind('<', close->end - 1);
        message.fields[name] = XmlUnescape(xml.substr(valueStart, valueEnd - valueStart));
    }
    return message;
}

std::string XmlEscape(std::string_view text)
{
    std::string out;
    out.reserve(text.size());
    for (const char c : text) {
        switch (c) {
        case '<': out += "&lt;"; break;
        case '>': out += "&gt;"; break;
        case '&': out += "&amp;"; break;
        case '"': out += "&quot;"; break;
        default: out += c; break;
        }
    }
    return out;
}

std::string BuildSoapRequest(std::string_view element, const SoapFields& fields)
{
    return BuildMessage(kRequestNamespaces, element, fields);
}

std::string BuildSoapResponse(std::string_view element, const SoapFields& fields)
{
    return BuildMessage(kResponseNamespaces, element, fields);
}

std::string BuildSoapFault(std::string_view faultCode, std::string_view message)
{
    std::string xml(kEnvelopeStart);
    xml += kResponseNamespaces;
    xml += "<SOAP-ENV:Fault><faultcode>";
    xml += XmlEscape(faultCode);
    xml += "</faultcode><faultstring>";
    xml += XmlEscape(message);
    xml += "</faultstring></SOAP-ENV:Fault>";
    xml += kEnvelopeClose;
    return xml;
}

} // namespace x2::deb
