// http_request.h

#pragma once
#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <iostream>
#include <map>
#include <string>

#include "nanonet/utility/log.h"

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

public:

    HttpRequest() {}

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

    inline size_t size() const {
        return body.size();
    }

    void setMethod(const std::string& method) {
        this->method = method;
        for (char& c : this->method)
            c = std::toupper(c);
    }

    const std::string& getMethod() const {
        return this->method;
    }

    void setVersion(const std::string& version) {
        this->version = version;
    }

    const std::string& getVersion() const {
        return this->version;
    }

    void setPath(const std::string& path) {
        this->path = path;
    }

    const std::string& getPath() const {
        return this->path;
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

    const std::map<const std::string, std::string>& getHeaders() const {
        return this->headers;
    }

    std::map<const std::string, std::string>& getHeaders() {
        return this->headers;
    }

    // body
    inline const std::string& getText() const {
        return body;
    }
    inline std::string& getText() {
        return body;
    }
    inline void setText(const std::string& newText) {
        this->body = newText;
    }
    inline void setText(std::string&& newText) {
        this->body = std::move(newText);
    }
    inline void appendText(const std::string& newText) {
        this->body += newText;
    }
    inline void appendText(std::string&& newText) {
        this->body += newText;
    }

    std::string toString() const {
        std::string request = method + ' ' + path + ' ' + version + "\r\n";

        for (const auto& e : headers)
            request += e.first + ": " + e.second + "\r\n";

        request += "\r\n" + body;
        return std::move(request);
    }

};  // class HttpRequest

}  // namespace nanonet

#endif  // __HTTP_REQUEST_H__