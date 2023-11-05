// http_request.hpp

#pragma once
#ifndef __HTTP_REQUEST_HPP__
#define __HTTP_REQUEST_HPP__

#include <iostream>
#include <map>
#include <string>

namespace nanonet {

class HttpRequest {
    // request line
    std::string method;
    std::string path;
    std::string version = "HTTP/1.0";

    // request headers
    std::map<const std::string, std::string> headers;
    std::string host;

    // request body
    std::string body;

    // https
    bool useSSL = false;

    void separateHostPath(const std::string& url, std::string& host, std::string& path) {
        // find the position of the double slash after the protocol
        size_t doubleSlashPos = url.find("//");
        if (doubleSlashPos != std::string::npos) {
            std::string protocol = url.substr(0, doubleSlashPos - 1);
            if (protocol == "https") {
                this->useSSL = true;
            } else if (protocol != "http") {
                Log::error << "[http] Unsupported protocols: \'"
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
            Log::error << "[http] invalid URL: \'" << url << '\'' << std::endl;
            exit(-1);
        }
    }

   public:
    HttpRequest(const std::string& method, const std::string& url) {
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

    void setHeader(const std::string& name, const std::string& value) {
        this->headers[name] = value;
    }

    void setBody(const std::string& body) {
        if (!body.empty()) {
            this->body = body;
            headers["Content-Length"] = std::to_string(body.size());
        }
    }

    bool usingSSL() const {
        return useSSL;
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

};  // class HttpRequest

}  // namespace nanonet

#endif  // __HTTP_REQUEST_HPP__