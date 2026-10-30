#include "Http.h"

#include "x2/platform/net_io.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace x2::epublisher {

namespace {

constexpr std::size_t kMaxHeader = 16 * 1024;
constexpr std::size_t kMaxBody = 1024 * 1024;
#if defined(MSG_NOSIGNAL)
constexpr int kSendFlags = MSG_NOSIGNAL;  // a closed peer must not raise SIGPIPE
#else
constexpr int kSendFlags = 0;
#endif

bool EqualsNoCase(std::string_view a, std::string_view b)
{
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
        return std::tolower(static_cast<unsigned char>(x)) == std::tolower(static_cast<unsigned char>(y));
    });
}

std::string_view Trim(std::string_view s)
{
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.remove_prefix(1);
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r')) s.remove_suffix(1);
    return s;
}

// Appends what arrives within the timeout; false when the peer closed, timed out or failed.
HttpReadResult Receive(SOCKET sock, unsigned timeoutMs, std::string& buffer)
{
    if (!x2::platform::net::WaitReadable(sock, timeoutMs))
        return HttpReadResult::Timeout;
    char chunk[4096];
    const int n = ::recv(sock, chunk, sizeof(chunk), 0);
    if (n <= 0)
        return HttpReadResult::Closed;
    buffer.append(chunk, static_cast<std::size_t>(n));
    return HttpReadResult::Ok;
}

} // namespace

HttpReadResult ReadHttpRequest(SOCKET sock, unsigned timeoutMs, std::string& pending, HttpRequest& request)
{
    std::size_t headerEnd;
    while ((headerEnd = pending.find("\r\n\r\n")) == std::string::npos) {
        if (pending.size() > kMaxHeader)
            return HttpReadResult::TooLarge;
        const auto r = Receive(sock, timeoutMs, pending);
        if (r != HttpReadResult::Ok)
            return pending.empty() && r == HttpReadResult::Closed ? HttpReadResult::Closed : r;
    }

    const std::string_view head(pending.data(), headerEnd);
    const auto lineEnd = head.find("\r\n");
    const std::string_view requestLine = head.substr(0, lineEnd);
    const auto sp1 = requestLine.find(' ');
    const auto sp2 = requestLine.find(' ', sp1 + 1);
    if (sp1 == std::string_view::npos || sp2 == std::string_view::npos)
        return HttpReadResult::Invalid;
    request.method = std::string(requestLine.substr(0, sp1));
    request.path = std::string(requestLine.substr(sp1 + 1, sp2 - sp1 - 1));
    const std::string_view version = requestLine.substr(sp2 + 1);
    request.keepAlive = version == "HTTP/1.1";

    std::size_t contentLength = 0;
    std::size_t pos = lineEnd == std::string_view::npos ? head.size() : lineEnd + 2;
    while (pos < head.size()) {
        auto next = head.find("\r\n", pos);
        if (next == std::string_view::npos) next = head.size();
        const std::string_view line = head.substr(pos, next - pos);
        pos = next + 2;
        const auto colon = line.find(':');
        if (colon == std::string_view::npos)
            continue;
        const std::string_view name = Trim(line.substr(0, colon));
        const std::string_view value = Trim(line.substr(colon + 1));
        if (EqualsNoCase(name, "Content-Length")) {
            contentLength = std::strtoul(std::string(value).c_str(), nullptr, 10);
        } else if (EqualsNoCase(name, "Connection")) {
            if (EqualsNoCase(value, "close")) request.keepAlive = false;
            else if (EqualsNoCase(value, "keep-alive")) request.keepAlive = true;
        } else if (EqualsNoCase(name, "Transfer-Encoding")) {
            return HttpReadResult::Invalid;  // gSOAP only chunks when asked to (SOAP_IO_CHUNK)
        }
    }
    if (contentLength > kMaxBody)
        return HttpReadResult::TooLarge;

    const std::size_t bodyStart = headerEnd + 4;
    while (pending.size() < bodyStart + contentLength) {
        const auto r = Receive(sock, timeoutMs, pending);
        if (r != HttpReadResult::Ok)
            return r == HttpReadResult::Closed ? HttpReadResult::Invalid : r;
    }
    request.body = pending.substr(bodyStart, contentLength);
    pending.erase(0, bodyStart + contentLength);
    return HttpReadResult::Ok;
}

bool WriteHttpResponse(SOCKET sock, int status, std::string_view reason, std::string_view contentType,
                       std::string_view body, bool keepAlive)
{
    std::string out = "HTTP/1.1 " + std::to_string(status) + ' ' + std::string(reason) + "\r\n";
    out += "Server: X2 AuthServer\r\n";
    out += "Content-Type: " + std::string(contentType) + "\r\n";
    out += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    out += keepAlive ? "Connection: keep-alive\r\n\r\n" : "Connection: close\r\n\r\n";
    out += body;
    std::size_t sent = 0;
    while (sent < out.size()) {
        const int n = ::send(sock, out.data() + sent, static_cast<int>(out.size() - sent), kSendFlags);
        if (n <= 0)
            return false;
        sent += static_cast<std::size_t>(n);
    }
    return true;
}

} // namespace x2::epublisher
