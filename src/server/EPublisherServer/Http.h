// The HTTP/1.1 subset the gSOAP clients of the EPublisher service use: one POST with Content-Length per connection (keep-alive is honoured
// when asked), answered with Content-Length.
#pragma once

#include "x2/platform/net.h"

#include <string>
#include <string_view>

namespace x2::epublisher {

struct HttpRequest
{
    std::string method;
    std::string path;
    std::string body;
    bool        keepAlive = false;
};

enum class HttpReadResult { Ok, Closed, Timeout, Invalid, TooLarge };

// Reads one request; `pending` keeps the bytes already received after it (pipelining/keep-alive).
HttpReadResult ReadHttpRequest(SOCKET sock, unsigned timeoutMs, std::string& pending, HttpRequest& request);

bool WriteHttpResponse(SOCKET sock, int status, std::string_view reason, std::string_view contentType,
                       std::string_view body, bool keepAlive);

} // namespace x2::epublisher
