// http/http_request.cpp

#include "nanonet/http/http_request.h"

namespace nanonet {

HttpRequest::HttpRequest() :path("/"), version("HTTP/1.0") {}

HttpRequest::HttpRequest(const std::string& method,
    const Url& url, const std::string& version) :version(version) {
    // set request-URI
    this->host = url.getHost();
    this->path = url.getAuthorityAfter();
    this->port = url.getPort();
    this->useSSL = url.getScheme() == "https";

    // set request method
    for (auto& ch : method) {
        // method to upper
        this->method.push_back(std::toupper(ch));
    }

    // set request headers
    this->headers["Accept"] = "*/*";
    this->headers["Host"] = host;
    this->headers["User-Agent"] = "Nanonet";
}

std::string HttpRequest::toString() const {
    std::string request = method + ' ' + path + ' ' + version + "\r\n";

    for (const auto& e : headers)
        request += e.first + ": " + e.second + "\r\n";

    request += "\r\n" + body;
    return std::move(request);
}

}  // namespace nanonet