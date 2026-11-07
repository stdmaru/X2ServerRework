// Minimal XML reader for the client's data files (AutoSetupGameOptionWeightConfig.xml, read by CX2GameOption): elements,
// attributes, text, comments, the XML declaration and processing instructions, CDATA, and the five predefined entities
// plus character references. No DTD, namespaces or validation; a <!DOCTYPE> is skipped. Text is kept as bytes (UTF-8 or
// whatever the file holds). Replaces libxml2, which the original linked for that one file.
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace x2::platform {

struct XmlElement
{
    std::string name;
    std::vector<std::pair<std::string, std::string>> attributes;   // in document order
    std::vector<XmlElement> children;                                // child elements, in document order
    std::string text;                                                // the element's own text, concatenated

    // The value of an attribute, or nullptr.
    const std::string* Attribute(std::string_view attributeName) const;
    // The first child element with this name, or nullptr.
    const XmlElement* Child(std::string_view childName) const;
};

// The root element of a document, or nullopt if it is not well formed (the error, with its byte offset, in `error`).
std::optional<XmlElement> ParseXml(std::string_view document, std::string* error = nullptr);

}  // namespace x2::platform
