// requests.hpp

#pragma once
#ifndef __REQUESTS_HPP__
#define __REQUESTS_HPP__

#include <iostream>
#include <string>
#include <map>

#include "http_request.hpp"
#include "http_respond.hpp"

namespace nanonet {

class Requests {
    static HttpRespond methods(const char* method, const std::string& url, const std::string& body) {
        HttpRequest httpRequest(method, url);
        if (!body.empty()) httpRequest.setBody(body);
        return send(httpRequest);
    }
public:
    static HttpRespond send(HttpRequest request) {
        // get addr and port from request
        std::string host = request.getHost();
        int slashPos = host.find(':');
        Port port = 80;
        if (slashPos != std::string::npos)
            port.val = static_cast<in_port_t>(std::stoi(host.substr(slashPos + 1)));
        Addr addr(host.substr(0, slashPos));

        // connnect to server
        Socket socket;
        socket.connect(addr, port);

        // send
        socket.send(request.toString());
    
        Log::debug << "[http] send http request to \'"
            << host << '\'' << std::endl;

        const int BUF_SIZE = 4096;
        char buffer[BUF_SIZE]{};
        socket.setReceiveTimeout(20, 0);

        // receive
        int recv_length = socket.receive(buffer, BUF_SIZE - 1);

        if (recv_length < 0) {
            // receive failed
            Log::warn << "[http] receive from \'"
                << host  << "\' timeout" << std::endl;
            socket.close();
            return {};
        } else if (recv_length == 0) {
            // server not responding
            Log::warn << "[http] server not responding" << std::endl;
            socket.close();
            return {};
        }

        HttpRespond respond;
        buffer[recv_length] = '\0';
        bool done = respond.append(buffer);
        while (!done && recv_length > 0) {
            socket.setReceiveTimeout(0, 200);
            recv_length = socket.receive(buffer, BUF_SIZE - 1);
            if (recv_length >= 0) buffer[recv_length] = '\0';
            done = respond.append(buffer);
        }

        Log::debug << "[http] receive from \'" << host
            << "\', text length = " << respond.size() << std::endl;
        socket.close();
        return respond;
    }
    inline static HttpRespond GET(const std::string& url, const std::string& body = "") {
        return methods("GET", url, body);
    }
    inline static HttpRespond HEAD(const std::string& url, const std::string& body = "") {
        return methods("HEAD", url, body);
    }
    inline static HttpRespond POST(const std::string& url, const std::string& body = "") {
        return methods("POST", url, body);
    }
    inline static HttpRespond PUT(const std::string& url, const std::string& body = "") {
        return methods("PUT", url, body);
    }
    inline static HttpRespond DELETE(const std::string& url, const std::string& body = "") {
        return methods("DELETE", url, body);
    }
    inline static HttpRespond CONNECT(const std::string& url, const std::string& body = "") {
        return methods("CONNECT", url, body);
    }
    inline static HttpRespond OPTIONS(const std::string& url, const std::string& body = "") {
        return methods("OPTIONS", url, body);
    }
    inline static HttpRespond TRACE(const std::string& url, const std::string& body = "") {
        return methods("TRACE", url, body);
    }
    inline static HttpRespond PATCH(const std::string& url, const std::string& body = "") {
        return methods("PATCH", url, body);
    }

}; // class Requests

} // namespace nanonet

#endif // __REQUESTS_HPP__