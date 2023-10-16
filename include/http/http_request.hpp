// http_request.hpp

#pragma once
#ifndef __HTTP_REQUEST_HPP__
#define __HTTP_REQUEST_HPP__

#include <iostream>
#include <string>
#include <map>

#define HTTP_GET        "GET"
#define HTTP_POST       "POST"
#define HTTP_PUT        "PUT"
#define HTTP_DELETE     "DELETE"
#define HTTP_HEAD       "HEAD"
#define HTTP_OPTIONS    "OPTIONS"
#define HTTP_TRACE      "TRACE"
#define HTTP_CONNECT    "CONNECT"

namespace nanonet {

class HTTPRequest {

    // request line
    std::string method;
    std::string path;
    std::string version;

    // request headers
    std::map<std::string, std::string> headers;
    std::string host;

    // request body
    std::string body;

    void separateHostPath(const std::string& url,std::string& host, std::string& path) {
        // find the position of the double slash after the protocol
        size_t doubleSlashPos = url.find("//");
        if (doubleSlashPos != std::string::npos) {
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
            std::cerr << "Invalid URL." << std::endl;
        }
    }

public:
    HTTPRequest(const std::string& method,
                 const std::string& url,
                 const std::string& version = "HTTP/1.1")
        :version(version) {

        // set request-URI
        separateHostPath(url, this->host, this->path);

        // set request method
        for (auto& ch : method) {
            // method to upper
            this->method.push_back(std::toupper(ch));
        }

        // set request headers
        headers["Connection"] = "close";
        headers["Content-Length"] = "0";
        headers["Host"] = host;
    }

    void setHeader(const std::string& name, const std::string& value) {
        headers[name] = value;
    }

    void setBody(const std::string& body) {
        this->body = body;
        headers["Content-Length"] = std::to_string(body.size());
    }

    std::string getHost() const {
        return this->host;
    }

    std::string toString() const {
        std::string request = method + ' ' + path + ' ' + version + "\r\n";

        for (const auto& [name, value] : headers)
            request += name + ": " + value + "\r\n";

        request += "\r\n" + body;
        return std::move(request);
    }

}; // class http_request

} // namespace nanonet

#endif // __HTTP_REQUEST_HPP__