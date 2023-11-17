// http/http_request.cpp

#include "nanonet/http/http_request.h"

namespace nanonet {

void HttpRequest::separateHostPath(const std::string& url, std::string& host, std::string& path) {
    // find the position of the double slash after the protocol
    size_t doubleSlashPos = url.find("//");
    if (doubleSlashPos != std::string::npos) {
        std::string protocol = url.substr(0, doubleSlashPos - 1);
        if (protocol == "https") {
            this->useSSL = true;
        } else if (protocol != "http") {
            error << "[http] Unsupported protocols: \'"
                << protocol << '\'' << std::endl;
            exit(-1);
        }
        // find the position of the first slash after the double slash
        size_t slashPos = url.find('/', doubleSlashPos + 2);
        if (slashPos != std::string::npos) {
            // fxtract the host
            host = url.substr(doubleSlashPos + 2, slashPos - doubleSlashPos - 2);
            // fxtract the path
            path = url.substr(slashPos);
        } else {
            // No path found, use root path
            host = url.substr(doubleSlashPos + 2);
            path = "/";
        }
    } else {
        error << "[http] invalid URL: \'" << url << '\'' << std::endl;
        exit(-1);
    }
}

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(const std::string& method, const std::string& url) {
    // set request-URI
    separateHostPath(url, this->host, this->path);

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