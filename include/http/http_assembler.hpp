// http_assembler.hpp

#pragma once
#ifndef __HTTP_ASSEMBLER_HPP__
#define __HTTP_ASSEMBLER_HPP__

#include <iostream>
#include <string>

#include "http/http_request.hpp"
#include "http/http_respond.hpp"

namespace nanonet {

class HttpAssembler {

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

    bool fillHead(HttpRespond& respond, const char* msg) {
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
            respond.setVersion(headCache.substr(0, versionEnd));
            size_t statusCodeEnd = headCache.find(' ', versionEnd + 1);
            respond.setStatusCode(headCache.substr(versionEnd + 1, statusCodeEnd - versionEnd - 1));
            respond.setStatusMessage(headCache.substr(statusCodeEnd + 1, pos - statusCodeEnd - 1));
            pos += 2;
            // get headers
            while (pos < headCache.size()) {
                size_t beginLine = pos;
                size_t endLine = headCache.find("\r\n", beginLine);
                size_t colon = headCache.find(": ", beginLine);
                if (colon > endLine) continue;
                respond.setHeader(
                    headCache.substr(beginLine, colon - beginLine),
                    headCache.substr(colon + 2, endLine - colon - 2)
                );
                pos = endLine + 2;
            }
            // set args
            const auto& headers = respond.getHeaders();
            auto it = headers.find("Content-Length");
            if (it != headers.end()) {
                headerContentLength = std::stoi(it->second);
            } else {
                it = headers.find("Transfer-Encoding");
                if (it != headers.end() && it->second == "chunked")
                    chunkedTransferEncoding = true;
            }
            // OK
            this->headDone = true;
            // fill body
            bool ret = appendBody(respond, bodyCache.c_str());
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
    inline static size_t hex2digit(const std::string& hexStr) {
        try {
            return (size_t)std::stol(hexStr, nullptr, 16);
        } catch (const std::exception& e) {
            return (size_t)std::string::npos;
        }
    }

    // append chunks when 'Transfer-Encoding' is 'chunked'
    bool appendChunk(HttpRespond& respond, const char* msg) {
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
                    respond.appendText(chunkCache.substr(chunkBase, appendLength));
                    chunkLast -= appendLength;
                    chunkBase = chunkPos = length;
                    return false;
                } else {
                    respond.appendText(chunkCache.substr(chunkBase, chunkLast));
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

    bool appendBody(HttpRespond& respond, const char* msg) {
        // when 'Transfer-Encoding' is 'chunked'
        if (chunkedTransferEncoding)
            return appendChunk(respond, msg);
        // append to body
        respond.appendText(msg);
        // when 'Content-Length' is set
        if (headerContentLength != std::string::npos) {
            if (respond.size() >= headerContentLength) {
                // receive done. truncate
                respond.getText().resize(headerContentLength);
                return isOK = true;
            } else {
                // continue
                return false;
            }
        }
        return false;
    }

public:

    bool append(HttpRespond& respond, const char* msg) {
        if (isOK)
            return true;

        if (headDone) {
            return appendBody(respond, msg);
        } else {
            return fillHead(respond, msg);
        }
    }


};  // class HttpAssembler

}  // namespace nanonet

#endif  // __HTTP_ASSEMBLER_HPP__