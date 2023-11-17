// http/http_assembler.cpp

#include "nanonet/http/http_assembler.h"

namespace nanonet {

template <class MsgType>
bool HttpAssembler<MsgType>::fillHead(HttpRespond& respond, const char* msg) {
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

template <class MsgType>
bool HttpAssembler<MsgType>::fillHead(HttpRequest& request, const char* msg) {
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
        size_t methodEnd = headCache.find(' ', 0);
        request.setMethod(headCache.substr(0, methodEnd));
        size_t pathEnd = headCache.find(' ', methodEnd + 1);
        request.setPath(headCache.substr(methodEnd + 1, pathEnd - methodEnd - 1));
        request.setVersion(headCache.substr(pathEnd + 1, pos - pathEnd - 1));
        pos += 2;
        // get headers
        while (pos < headCache.size()) {
            size_t beginLine = pos;
            size_t endLine = headCache.find("\r\n", beginLine);
            size_t colon = headCache.find(": ", beginLine);
            if (colon > endLine) continue;
            request.setHeader(
                headCache.substr(beginLine, colon - beginLine),
                headCache.substr(colon + 2, endLine - colon - 2)
            );
            pos = endLine + 2;
        }
        // set args
        const auto& headers = request.getHeaders();
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
        bool ret = appendBody(bodyCache.c_str());
        bodyCache.clear();
        if (request.getMethod() == "GET")
            return isOK = true;
        return ret;
    }
    // cannot found '\r\n\r\n', set find start pos to size - 4
    bodyStartCache = headCache.size() - 4;
    if (bodyStartCache < 0)
        bodyStartCache = 0;
    return false;
}

// converts hex text to digit, on error, -1 is returned
template <class MsgType>
size_t HttpAssembler<MsgType>::hex2digit(const std::string& hexStr) {
    try {
        return (size_t)std::stol(hexStr, nullptr, 16);
    } catch (const std::exception& e) {
        return (size_t)std::string::npos;
    }
}

// append chunks when 'Transfer-Encoding' is 'chunked'
template <class MsgType>
bool HttpAssembler<MsgType>::appendChunk(const char* msg) {
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
                httpmsg.appendText(chunkCache.substr(chunkBase, appendLength));
                chunkLast -= appendLength;
                chunkBase = chunkPos = length;
                return false;
            } else {
                httpmsg.appendText(chunkCache.substr(chunkBase, chunkLast));
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

template <class MsgType>
bool HttpAssembler<MsgType>::appendBody(const char* msg) {
    // when 'Transfer-Encoding' is 'chunked'
    if (chunkedTransferEncoding)
        return appendChunk(msg);
    // append to body
    httpmsg.appendText(msg);
    // when 'Content-Length' is set
    if (headerContentLength != std::string::npos) {
        if (httpmsg.size() >= headerContentLength) {
            // receive done. truncate
            httpmsg.getText().resize(headerContentLength);
            return isOK = true;
        } else {
            // continue
            return false;
        }
    }
    return false;
}


template <class MsgType>
HttpAssembler<MsgType>::HttpAssembler(MsgType& httpmsg) :httpmsg(httpmsg) {}

template <class MsgType>
bool HttpAssembler<MsgType>::append(const char* msg) {
    if (isOK)
        return true;

    if (headDone) {
        return appendBody(msg);
    } else {
        return fillHead(httpmsg, msg);
    }
}

}  // namespace nanonet