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
    std::string bodyCache;
    std::string headCache;
    std::string chunkCache;
    size_t bodyStart = std::string::npos;
    size_t bodyStartCache = 0;
    size_t headerContentLength = std::string::npos;
    size_t chunkLast = 0;
    size_t chunkPos = 0;
    size_t chunkBase = 0;
    bool chunkedTransferEncoding = false;
    bool isOK = false;
    bool headDone = false;

private:

    bool fillHead(const char* msg) {
        headCache += msg;
        bodyStart = headCache.find("\r\n\r\n", bodyStartCache);
        // header received in its entirety, decode the header
        if (bodyStart != std::string::npos) {
            bodyStart += 4;
            // separate the head (line, headders)
            bodyCache += headCache.substr(bodyStart);
            headCache.resize(bodyStart - 2);
            // get line
            size_t pos = headCache.find("\r\n");
            size_t versionEnd = headCache.find(' ', 0);
            this->version = headCache.substr(0, versionEnd);
            size_t statusCodeEnd = headCache.find(' ', versionEnd + 1);
            this->statusCode = headCache.substr(versionEnd + 1, statusCodeEnd - versionEnd - 1);
            this->statusMessage = headCache.substr(statusCodeEnd + 1, pos - statusCodeEnd - 1);
            pos += 2;
            // get headers
            while (pos < headCache.size()) {
                size_t beginLine = pos;
                size_t endLine = headCache.find("\r\n", beginLine);
                size_t colon = headCache.find(": ", beginLine);
                if (colon > endLine) continue;
                headers[headCache.substr(beginLine, colon - beginLine)]
                    = headCache.substr(colon + 2, endLine - colon - 2);
                pos = endLine + 2;
            }
            // set args
            auto it = headers.find("Content-Length");
            if (it != headers.end()) {
                headerContentLength = std::stoi(it->second);
            } else {
                it = headers.find("Transfer-Encoding");
                if (it->second == "chunked")
                    chunkedTransferEncoding = true;
            }
            // OK
            this->headDone = true;
            // fill body
            bool ret = appendBody(bodyCache.c_str());
            bodyCache.clear();
            return ret;
        }
        // cannot found '\r\n\r\n', set find start pos to size - 4
        bodyStartCache = headCache.size() - 4;
        if (bodyStartCache < 0)
            bodyStartCache = 0;
        return false;
    }

    // converts hex text to digit, on error, -1 is returned
    static size_t hex2digit(const std::string& hexStr) {
        try {
            return (size_t)std::stol(hexStr, nullptr, 16);
        } catch (const std::exception& e) {
            return (size_t)std::string::npos;
        }
    }

    // append chunks when 'Transfer-Encoding' is 'chunked'
    bool appendChunk(const char* msg) {
        chunkCache += msg;
        size_t length = chunkCache.size();
        while (chunkBase < length) {
            if (chunkLast == 0) {
                if ((chunkPos = chunkCache.find("\r\n", chunkBase)) != std::string::npos) {
                    size_t chunkLength = hex2digit(
                        chunkCache.substr(chunkBase, chunkPos - chunkBase));
                    if (chunkLength == 0) {
                        chunkCache.clear();
                        return isOK = true;
                    }
                    chunkLast = chunkLength;
                    chunkBase = chunkPos += 2;
                    if (chunkBase >= length)
                        return false;
                } else {
                    return false;
                }
            } else {
                size_t appendLength = length - chunkBase;
                if (appendLength <= chunkLast) {
                    body += chunkCache.substr(chunkBase, appendLength);
                    chunkLast -= appendLength;
                    chunkBase = chunkPos = length;
                    return false;
                } else {
                    body += chunkCache.substr(chunkBase, chunkLast);
                    chunkBase += chunkLast + 2;
                    chunkLast = 0;
                }
                // clear cache
                if (chunkBase == chunkCache.size()) {
                    chunkCache.clear();
                    chunkBase = 0;
                }
            }
        }
        return false;
    }

    bool appendBody(const char* msg) {
        // when 'Transfer-Encoding' is 'chunked'
        if (chunkedTransferEncoding)
            return appendChunk(msg);
        // append to body
        body += msg;
        // when 'Content-Length' is set
        if (headerContentLength != std::string::npos) {
            if (body.size() >= headerContentLength) {
                // receive done. truncate
                body.resize(headerContentLength);
                return isOK = true;
            } else {
                // continue
                return false;
            }
        }
        return false;
    }


public:

    HttpRespond() {}

    // append to respond message from received
    bool append(const char* msg) {
        if (isOK)
            return true;

        if (headDone) {
            return appendBody(msg);
        } else {
            return fillHead(msg);
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
            // Log::warn << "header \'" << headerName << "\' not found" << std::endl;
            return notFound;
        }
    }

    bool findHeader(const std::string& headerName) const {
        return this->headers.find(headerName) != this->headers.end();
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