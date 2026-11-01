#include "SoapClient.h"

#include "x2/platform/net.h"
#include "x2/platform/net_io.h"
#include "x2/platform/unicode.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>

namespace x2::deb {

namespace {

constexpr std::size_t kMaxResponse = 1024 * 1024;
#if defined(MSG_NOSIGNAL)
constexpr int kSendFlags = MSG_NOSIGNAL;  // a closed peer must not raise SIGPIPE
#else
constexpr int kSendFlags = 0;
#endif

struct Url
{
    std::string    host;
    unsigned short port = 80;
    std::string    path = "/";
};

bool ParseUrl(std::string_view url, Url& out)
{
    constexpr std::string_view scheme = "http://";
    if (url.substr(0, scheme.size()) != scheme)
        return false;  // gSOAP also took https://, which the DEB service never used
    url.remove_prefix(scheme.size());
    const auto slash = url.find('/');
    std::string_view authority = url.substr(0, slash);
    out.path = slash == std::string_view::npos ? "/" : std::string(url.substr(slash));
    const auto colon = authority.rfind(':');
    if (colon != std::string_view::npos) {
        const std::string port(authority.substr(colon + 1));
        char* end = nullptr;
        const unsigned long value = std::strtoul(port.c_str(), &end, 10);
        if (port.empty() || *end != '\0' || value == 0 || value > 65535)
            return false;
        out.port = static_cast<unsigned short>(value);
        authority = authority.substr(0, colon);
    }
    out.host = std::string(authority);
    return !out.host.empty();
}

std::string SocketError()
{
    namespace net = x2::platform::net;
    return x2::platform::WideToUtf8(net::SocketErrorMessage(net::LastSocketError()));
}

bool EqualsNoCase(std::string_view a, std::string_view b)
{
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
        return std::tolower(static_cast<unsigned char>(x)) == std::tolower(static_cast<unsigned char>(y));
    });
}

class Socket
{
public:
    Socket() : m_sock(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) {}
    ~Socket() { if (m_sock != INVALID_SOCKET) closesocket(m_sock); }
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    SOCKET Get() const { return m_sock; }

private:
    SOCKET m_sock;
};

// Value of a header of the response head (data[0, headerEnd)), or nullopt.
std::optional<std::string_view> HeaderValue(std::string_view data, std::size_t headerEnd, std::string_view name)
{
    std::size_t pos = data.find("\r\n") + 2;
    while (pos < headerEnd) {
        const auto next = data.find("\r\n", pos);
        const std::string_view line = data.substr(pos, next - pos);
        pos = next + 2;
        const auto colon = line.find(':');
        if (colon == std::string_view::npos || !EqualsNoCase(line.substr(0, colon), name))
            continue;
        std::string_view value = line.substr(colon + 1);
        while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) value.remove_prefix(1);
        return value;
    }
    return std::nullopt;
}

// Size of the whole response once its head arrived and it has a Content-Length; nullopt while unknown (it then ends with
// the connection).
std::optional<std::size_t> ResponseSize(std::string_view data)
{
    const auto headerEnd = data.find("\r\n\r\n");
    if (headerEnd == std::string_view::npos)
        return std::nullopt;
    const auto length = HeaderValue(data, headerEnd, "Content-Length");
    if (!length)
        return std::nullopt;
    return headerEnd + 4 + std::strtoul(std::string(*length).c_str(), nullptr, 10);
}

// Status code and body of an HTTP/1.x response that ends at Content-Length or at the close of the connection.
bool ParseResponse(std::string_view data, int& status, std::string& body, std::string& error)
{
    const auto headerEnd = data.find("\r\n\r\n");
    if (headerEnd == std::string_view::npos || data.substr(0, 5) != "HTTP/") {
        error = "malformed HTTP response";
        return false;
    }
    const auto sp = data.find(' ');
    status = sp < headerEnd ? std::atoi(std::string(data.substr(sp + 1, 3)).c_str()) : 0;
    if (const auto encoding = HeaderValue(data, headerEnd, "Transfer-Encoding"); encoding && !EqualsNoCase(*encoding, "identity")) {
        error = "chunked HTTP response";  // the service answers with Content-Length (gSOAP did too)
        return false;
    }
    const std::size_t size = ResponseSize(data).value_or(data.size());
    if (size > data.size()) {
        error = "truncated HTTP response";
        return false;
    }
    body = std::string(data.substr(headerEnd + 4, size - headerEnd - 4));
    return true;
}

} // namespace

SoapClient::SoapClient(std::string endpoint, unsigned timeoutMs)
    : m_endpoint(std::move(endpoint))
    , m_timeoutMs(timeoutMs)
{
}

std::optional<SoapMessage> SoapClient::Call(std::string_view element, const SoapFields& fields, std::string& error) const
{
    Url target;
    if (!ParseUrl(m_endpoint, target)) {
        error = "invalid endpoint " + m_endpoint;
        return std::nullopt;
    }
    std::string ip = target.host;
    if (x2::platform::IpFromString(ip.c_str()) == INADDR_NONE) {
        ip = x2::platform::ResolveIPv4(target.host.c_str());
        if (ip.empty()) {
            error = "cannot resolve " + target.host;
            return std::nullopt;
        }
    }

    Socket sock;
    if (sock.Get() == INVALID_SOCKET) {
        error = "socket: " + SocketError();
        return std::nullopt;
    }
    SOCKADDR_IN addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(target.port);
    addr.sin_addr.s_addr = x2::platform::IpFromString(ip.c_str());
    // Blocking connect, as gSOAP's (connect_timeout 0): the service is on the same machine or the local network.
    if (::connect(sock.Get(), reinterpret_cast<const SOCKADDR*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        error = "connect " + ip + ':' + std::to_string(target.port) + ": " + SocketError();
        return std::nullopt;
    }

    const std::string body = BuildSoapRequest(element, fields);
    std::string request = "POST " + target.path + " HTTP/1.1\r\n";
    request += "Host: " + target.host + ':' + std::to_string(target.port) + "\r\n";
    request += "User-Agent: gSOAP/2.7\r\n";  // what the service saw from the original client
    request += "Content-Type: text/xml; charset=utf-8\r\n";
    request += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    request += "Connection: close\r\n";
    request += "SOAPAction: \"\"\r\n\r\n";
    request += body;
    for (std::size_t sent = 0; sent < request.size();) {
        const int n = ::send(sock.Get(), request.data() + sent, static_cast<int>(request.size() - sent), kSendFlags);
        if (n <= 0) {
            error = "send: " + SocketError();
            return std::nullopt;
        }
        sent += static_cast<std::size_t>(n);
    }

    std::string response;
    for (;;) {
        if (!x2::platform::net::WaitReadable(sock.Get(), m_timeoutMs)) {
            error = "no answer within " + std::to_string(m_timeoutMs) + " ms";
            return std::nullopt;
        }
        char chunk[4096];
        const int n = ::recv(sock.Get(), chunk, sizeof(chunk), 0);
        if (n < 0) {
            error = "recv: " + SocketError();
            return std::nullopt;
        }
        if (n == 0)
            break;  // "Connection: close": the answer ends with the connection
        response.append(chunk, static_cast<std::size_t>(n));
        if (response.size() > kMaxResponse) {
            error = "HTTP response too large";
            return std::nullopt;
        }
        if (const auto size = ResponseSize(response); size && response.size() >= *size)
            break;  // complete, even if the service keeps the connection open
    }

    int status = 0;
    std::string content;
    if (!ParseResponse(response, status, content, error))
        return std::nullopt;
    auto message = ParseSoapMessage(content);
    if (message && message->element == "Fault") {
        error = "SOAP Fault " + message->Text("faultcode") + ": " + message->Text("faultstring");
        return std::nullopt;
    }
    if (status != 200) {
        error = "HTTP " + std::to_string(status);
        return std::nullopt;
    }
    if (!message) {
        error = "malformed SOAP response";
        return std::nullopt;
    }
    return message;
}

} // namespace x2::deb
