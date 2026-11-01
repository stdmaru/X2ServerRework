// SOAP 1.1 document/literal messages of the DEB "AuthService" (namespace urn:AuthService), written and read as the gSOAP 2.7
// client the servers used to have (decision T31): the servers' KDEBAuth/KDEBBilling send the requests, the EPublisherServer
// (decision T23) answers them. Only what that service exchanges is supported: one element inside Body, with simple child elements.
#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace x2::deb {

inline constexpr std::string_view kServiceNamespace = "urn:AuthService";

struct SoapMessage
{
    std::string                        element;  // local name of the element in Body ("LoginRequest", "Fault"...)
    std::map<std::string, std::string> fields;   // child elements by local name, XML entities decoded

    // A missing or malformed field reads as empty / 0, as gSOAP left the members it did not find at their default.
    const std::string& Text(const char* name) const;
    long long          Int(const char* name) const;
    std::uint64_t      UInt(const char* name) const;
};

// nullopt when the text is not a SOAP envelope with an element in Body.
std::optional<SoapMessage> ParseSoapMessage(std::string_view xml);

using SoapFields = std::vector<std::pair<std::string, std::string>>;

// <SOAP-ENV:Envelope ...><SOAP-ENV:Body><ns2:<element>><ns2:<name>>value</...>... (values are escaped). The request carries
// the same namespace declarations gSOAP wrote (with the ns1 of the WSDL), so a service written for that client still reads it.
std::string BuildSoapRequest(std::string_view element, const SoapFields& fields);
std::string BuildSoapResponse(std::string_view element, const SoapFields& fields);
std::string BuildSoapFault(std::string_view faultCode, std::string_view message);

std::string XmlEscape(std::string_view text);

} // namespace x2::deb
