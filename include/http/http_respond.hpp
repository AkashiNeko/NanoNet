// http_respond.hpp

#pragma once
#ifndef __HTTP_RESPOND_HPP__
#define __HTTP_RESPOND_HPP__

// C
#include <cassert>

// C++
#include <string>
#include <unordered_map>
#include <sstream>

namespace nanonet {

class HTTPRespond {

    // line
    std::string version;
    std::string statusCode;
    std::string statusMessage;

    // headders
    std::unordered_map<std::string, std::string> headers;

    // text
    std::string body;

    // check
    std::string head;
    size_t bodyStart = std::string::npos;
    size_t contentLength = 0;
    bool isValid = true;

public:

    HTTPRespond() {}

    bool append(const char* msg) {
        // check if the complete header has been received
        if (bodyStart == std::string::npos) {
            head += msg;
            bodyStart = head.find("\r\n\r\n") + 4;
            // header received in its entirety, decode the header
            if (bodyStart != std::string::npos) {
                // separate the head (line, headders)
                body += head.substr(bodyStart);
                body.resize(bodyStart - 2);

                size_t pos = head.find("\r\n");
                std::string respondLine = head.substr(0, pos);
                size_t versionEnd = respondLine.find(' ');
                this->version = respondLine.substr(0, versionEnd);
                size_t statusCodeEnd = respondLine.find(' ', versionEnd + 1);
                这里还没写完
            }
        } else {
            // the complete header has been received
            body += msg;
        }
    }

    size_t size() const {
        return body.size();
    }

    int getStatusCode() const {
        return statusCode;
    }

    std::string getVersion() const {
        return version;
    }

    std::string getStatusMessage() const {
        return statusMessage;
    }

    std::string getText() const {
        return body;
    }

    std::string getHeader(const std::string& headerName) const {
        if (headers.count(headerName) > 0) {
            return headers.at(headerName);
        }
        return std::string();
    }

    std::string toString() const {
        std::string result = version + ' ' + std::to_string(statusCode) + ' ' + statusMessage + "\r\n";

        for (const auto& [name, value] : headers)
            result += name + ": " + value + "\r\n";

        result += "\r\n" + body;
        return std::move(result);
    }

}; // class HTTPRespond

} // namespace nanonet

#endif // __HTTP_RESPOND_HPP__