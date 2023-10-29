// requests.hpp

#pragma once
#ifndef __REQUESTS_HPP__
#define __REQUESTS_HPP__

#include <iostream>
#include <string>
#include <map>

#include "http_request.hpp"
#include "http_respond.hpp"
#include "requestor.hpp"

#define HTTP_GET        "GET"
#define HTTP_POST       "POST"
#define HTTP_PUT        "PUT"
#define HTTP_DELETE     "DELETE"
#define HTTP_HEAD       "HEAD"
#define HTTP_OPTIONS    "OPTIONS"
#define HTTP_TRACE      "TRACE"
#define HTTP_CONNECT    "CONNECT"

namespace nanonet {

class Requests {
public:
    inline static HTTPRespond get(const std::string& url) {
        HTTPRequest httpRequest("GET", url);
        std::cout << httpRequest.toString() << std::endl;
        return Requestor::send(httpRequest);
    }
    inline static HTTPRespond post(const std::string& url, const std::string& body = "") {
        HTTPRequest httpRequest(HTTP_GET, url);
        httpRequest.setBody(body);
        return Requestor::send(httpRequest);
    }

}; // class Requests

} // namespace nanonet

#endif // __REQUESTS_HPP__