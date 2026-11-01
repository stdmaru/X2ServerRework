// Blocking SOAP client of the DEB AuthService (decision T31), in place of the gSOAP one: one HTTP/1.1 POST per call on a new
// connection, with the request line and headers gSOAP 2.7 sent ("Connection: close", empty SOAPAction).
#pragma once

#include "Soap.h"

#include <string>
#include <string_view>

namespace x2::deb {

class SoapClient
{
public:
    // endpoint: "http://host[:port][/path]" (the address the Lua config gives, AddAuthServiceAddress/AddWebServiceAddress).
    explicit SoapClient(std::string endpoint, unsigned timeoutMs = 10 * 1000);

    // The element in the answer's Body, or nullopt with `error` filled (network, HTTP or SOAP Fault).
    std::optional<SoapMessage> Call(std::string_view element, const SoapFields& fields, std::string& error) const;

    const std::string& Endpoint() const { return m_endpoint; }

private:
    std::string m_endpoint;
    unsigned    m_timeoutMs;
};

} // namespace x2::deb
