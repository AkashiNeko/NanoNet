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

namespace nanonet {

class Requests {
    static HTTPRespond methods(const char* method, const std::string& url, const std::string& body) {
        HTTPRequest httpRequest(method, url);
        httpRequest.setBody(body);
        return send(httpRequest);
    }
public:
    static HTTPRespond send(HTTPRequest request) {
        // get addr and port from request
        std::string host = request.getHost();
        int slashPos = host.find(':');
        Port port = 80;
        if (slashPos != std::string::npos) {
            port.val = static_cast<in_port_t>(std::stoi(host.substr(slashPos + 1)));
        }
        Addr addr(host.substr(0, slashPos));

        // connnect to server
        Socket socket;
        socket.connect(addr, port);

        // send
        socket.send(request.toString());
    
        Log::debug << "[http] send http request to \'" << host
            << ":" << port.toString() << '\'' << std::endl;

        const int BUF_SIZE = 4096;
        char buffer[BUF_SIZE]{};
        socket.setReceiveTimeout(20, 0);

        // receive
        int recv_length = socket.receive(buffer, BUF_SIZE - 1);

        if (recv_length < 0) {
            Log::warn << "[http] receive from \'" << host << ":"
                << port.toString() << "\' timeout" << std::endl;
            socket.close();
            return {};
        } else if (recv_length == 0) {
            Log::warn << "[http] server not responding" << std::endl;
            socket.close();
            return {};
        }

        HTTPRespond respond;
        respond.append(buffer);
        while (recv_length > 0) {
            socket.setReceiveTimeout(0, 100);
            recv_length = socket.receive(buffer, BUF_SIZE - 1);
            if (recv_length >= 0) buffer[recv_length] = '\0';
            bool done = respond.append(buffer);
            if (done) break;
        }
        socket.close();
        Log::debug << "[http] receive from \'" << host << ":" << port.toString()
            << "\', text length = " << respond.size() << std::endl;
        return respond;
    }
    inline static HTTPRespond GET(const std::string& url, const std::string& body = "") {
        return methods("GET", url, body);
    }
    inline static HTTPRespond HEAD(const std::string& url, const std::string& body = "") {
        return methods("HEAD", url, body);
    }
    inline static HTTPRespond POST(const std::string& url, const std::string& body = "") {
        return methods("POST", url, body);
    }
    inline static HTTPRespond PUT(const std::string& url, const std::string& body = "") {
        return methods("PUT", url, body);
    }
    inline static HTTPRespond DELETE(const std::string& url, const std::string& body = "") {
        return methods("DELETE", url, body);
    }
    inline static HTTPRespond CONNECT(const std::string& url, const std::string& body = "") {
        return methods("CONNECT", url, body);
    }
    inline static HTTPRespond OPTIONS(const std::string& url, const std::string& body = "") {
        return methods("OPTIONS", url, body);
    }
    inline static HTTPRespond TRACE(const std::string& url, const std::string& body = "") {
        return methods("TRACE", url, body);
    }
    inline static HTTPRespond PATCH(const std::string& url, const std::string& body = "") {
        return methods("PATCH", url, body);
    }

}; // class Requests

} // namespace nanonet

#endif // __REQUESTS_HPP__