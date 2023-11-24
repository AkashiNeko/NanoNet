// requests.h

#pragma once
#ifndef __REQUESTS_H__
#define __REQUESTS_H__

#include <iostream>
#include <string>

#include "nanonet/tcp/socket.h"
#include "nanonet/http/http_assembler.h"
#include "nanonet/http/http_request.h"
#include "nanonet/http/http_respond.h"
#include "nanonet/utility/addr_port.h"
#include "nanonet/utility/url.h"
#include "nanonet/exception/nanoerr.h"

namespace nanonet {

class Requests {

    // send method
    static HttpRespond _methods(const char* method, const Url& url, const std::string& body);

public:

    // get addr and port from request
    static AddrPort getAddrPortFromHost(const std::string& host, bool useSSL = false);

    // send request
    static HttpRespond send(HttpRequest request, int timeout = 10);

    // methods
    inline static HttpRespond GET(const Url& url, const std::string& body = "") {
        return _methods("GET", url, body);
    }
    inline static HttpRespond HEAD(const Url& url, const std::string& body = "") {
        return _methods("HEAD", url, body);
    }
    inline static HttpRespond POST(const Url& url, const std::string& body = "") {
        return _methods("POST", url, body);
    }
    inline static HttpRespond PUT(const Url& url, const std::string& body = "") {
        return _methods("PUT", url, body);
    }
    inline static HttpRespond DELETE(const Url& url, const std::string& body = "") {
        return _methods("DELETE", url, body);
    }
    inline static HttpRespond CONNECT(const Url& url, const std::string& body = "") {
        return _methods("CONNECT", url, body);
    }
    inline static HttpRespond OPTIONS(const Url& url, const std::string& body = "") {
        return _methods("OPTIONS", url, body);
    }
    inline static HttpRespond TRACE(const Url& url, const std::string& body = "") {
        return _methods("TRACE", url, body);
    }
    inline static HttpRespond PATCH(const Url& url, const std::string& body = "") {
        return _methods("PATCH", url, body);
    }

};  // class Requests

}  // namespace nanonet

#endif  // __REQUESTS_H__