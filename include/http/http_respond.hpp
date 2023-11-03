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

class HttpRespond {

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
    size_t contentLength = -1;
    bool isOK = false;

public:

    HttpRespond() {}

    bool append(const char* msg) {
        if (isOK) return true;
        // check if the complete header has been received
        if (bodyStart == std::string::npos) {
            head += msg;
            bodyStart = head.find("\r\n\r\n");
            if (bodyStart != std::string::npos) bodyStart += 4;
            // header received in its entirety, decode the header
            if (bodyStart != std::string::npos) {
                // separate the head (line, headders)
                body += head.substr(bodyStart);
                head.resize(bodyStart - 2);
                // get line
                size_t pos = head.find("\r\n");
                size_t versionEnd = head.find(' ', 0);
                this->version = head.substr(0, versionEnd);
                size_t statusCodeEnd = head.find(' ', versionEnd + 1);
                this->statusCode = head.substr(versionEnd + 1, statusCodeEnd - versionEnd - 1);
                this->statusMessage = head.substr(statusCodeEnd + 1, pos - statusCodeEnd - 1);
                pos += 2;
                while (pos < head.size()) {
                    size_t beginLine = pos;
                    size_t endLine = head.find("\r\n", beginLine);
                    size_t colon = head.find(": ", beginLine);
                    if (colon > endLine) continue;
                    headers[head.substr(beginLine, colon - beginLine)] = head.substr(colon + 2, endLine - colon - 2);
                    pos = endLine + 2;
                }
                auto it = headers.find("Content-Length");
                if (it != headers.end())
                    contentLength = std::stoi(it->second);
                if (body.size() >= contentLength) {
                    body.resize(contentLength);
                    return isOK = true;
                }
            }
            return false;
        } else {
            // the complete header has been received
            body += msg;
            // body length >= 'Content-Length'
            if (contentLength != -1 && body.size() >= contentLength) {
                body.resize(contentLength);
                return isOK = true;
            }
            return false;
        }
    }

    size_t size() const {
        return body.size();
    }

    const std::string& getStatusCode() const {
        return statusCode;
    }

    const std::string& getVersion() const {
        return version;
    }

    const std::string& getStatusMessage() const {
        return statusMessage;
    }

    const std::string& getText() const {
        return body;
    }

    std::string& getText() {
        return body;
    }

    const std::string& getHeader(const std::string& headerName) const {
        static std::string notFound = "header not found";
        auto it = headers.find(headerName);
        if (it != headers.end()) {
            return it->second;
        } else {
            Log::warn << "header \'" << headerName << "\' not found" << std::endl;
            return notFound;
        }
    }

    const std::unordered_map<std::string, std::string>& getHeaders() const {
        return headers;
    }

    std::string toString() const {
        std::string result = version + ' ' + statusCode + ' ' + statusMessage + "\r\n";

        for (const auto& [name, value] : headers)
            result += name + ": " + value + "\r\n";

        result += "\r\n" + body;
        return std::move(result);
    }

}; // class HttpRespond

} // namespace nanonet

#endif // __HTTP_RESPOND_HPP__