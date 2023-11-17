// http_assembler.h

#pragma once
#ifndef __HTTP_ASSEMBLER_H__
#define __HTTP_ASSEMBLER_H__

#include <iostream>
#include <string>

#include "nanonet/http/http_request.h"
#include "nanonet/http/http_respond.h"

namespace nanonet {

template <class MsgType>
class HttpAssembler {

    MsgType& httpmsg;

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

    bool fillHead(HttpRespond& respond, const char* msg);
    bool fillHead(HttpRequest& request, const char* msg);

    // converts hex text to digit, on error, -1 is returned
    static size_t hex2digit(const std::string& hexStr) {
        try {
            return (size_t)std::stol(hexStr, nullptr, 16);
        } catch (const std::exception& e) {
            return (size_t)std::string::npos;
        }
    }

    // append chunks when 'Transfer-Encoding' is 'chunked'
    bool appendChunk(const char* msg);

    bool appendBody(const char* msg);

public:

    HttpAssembler(MsgType& httpmsg);

    bool append(const char* msg);

};  // class HttpAssembler

template class HttpAssembler<HttpRequest>;
template class HttpAssembler<HttpRespond>;

}  // namespace nanonet

#endif  // __HTTP_ASSEMBLER_H__