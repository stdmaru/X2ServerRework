// SOAP 1.1 document/literal messages of the EPublisher service (the DEB "AuthService", namespace urn:AuthService), as the gSOAP 2.7
// client of the servers writes and reads them (Common/OnlyGlobal/AuthAndBilling/DEB). Only what that client
// produces is supported: one operation element inside Body, with simple child elements.
#pragma once

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace x2::epublisher {

inline constexpr std::string_view kServiceNamespace = "urn:AuthService";

struct SoapRequest
{
    std::string                        operation;  // local name of the element in Body ("LoginRequest"...)
    std::map<std::string, std::string> fields;     // child elements by local name, XML entities decoded
};

// nullopt when the text is not a SOAP envelope with an element in Body.
std::optional<SoapRequest> ParseSoapRequest(std::string_view xml);

using SoapFields = std::vector<std::pair<std::string, std::string>>;

// <SOAP-ENV:Envelope ...><SOAP-ENV:Body><ns2:<element>><ns2:<name>>value</...>... (values are escaped)
std::string BuildSoapResponse(std::string_view element, const SoapFields& fields);
std::string BuildSoapFault(std::string_view faultCode, std::string_view message);

std::string XmlEscape(std::string_view text);

} // namespace x2::epublisher
