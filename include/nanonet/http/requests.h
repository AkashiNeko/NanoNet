// requests.h

#pragma once
#ifndef __REQUESTS_H__
#define __REQUESTS_H__

#include <iostream>
#include <string>

#include "nanonet/tcp/socket.h"
#include "nanonet/http/http_request.h"
#include "nanonet/http/http_respond.h"
#include "nanonet/utility/addr_port.h"

namespace nanonet {

class Requests {

    // send method
    static HttpRespond methods(const char* method, const std::string& url, const std::string& body) {
        HttpRequest httpRequest(method, url);
        if (!body.empty())
            httpRequest.setBody(body);
        return send(httpRequest);
    }

public:

    // get addr and port from request
    static AddrPort getAddrPortFromHost(const std::string& host, bool useSSL = false) {
        int slashPos = host.find(':');
        Port port = useSSL ? 443 : 80;
        if (slashPos != std::string::npos)
            port = static_cast<in_port_t>(std::stoi(host.substr(slashPos + 1)));
        Addr addr(host.substr(0, slashPos));
        return AddrPort(addr, port);
    }

    // send request
    static HttpRespond send(HttpRequest request, int timeout = 10) {
        std::string host = request.getHost();
        AddrPort addrPort = getAddrPortFromHost(host, request.usingSSL());

        // connnect to server
        Socket socket;
        socket.connect(addrPort);

        // send
        socket.send(request.toString());

        debug << "[http] send http request to \'"
            << host << '\'' << std::endl;

        const int BUF_SIZE = 4096;
        char buffer[BUF_SIZE]{};
        socket.setReceiveTimeout(timeout, 0);

        // receive
        int receiveLength = socket.receive(buffer, BUF_SIZE - 1);

        if (receiveLength < 0) {
            // receive failed
            warn << "[http] receive from \'"
                << host << "\' timeout" << std::endl;
            socket.close();
            return {};
        } else if (receiveLength == 0) {
            // server not responding
            warn << "[http] server not responding" << std::endl;
            socket.close();
            return {};
        }
        buffer[receiveLength] = '\0';

        HttpRespond respond;
        HttpAssembler assembler;
        bool receiveDone = assembler.append(respond, buffer);
        while (!receiveDone && receiveLength > 0) {
            socket.setReceiveTimeout(5, 0);
            buffer[0] = '\0';
            receiveLength = socket.receive(buffer, BUF_SIZE - 1);
            if (receiveLength >= 0)
                buffer[receiveLength] = '\0';
            receiveDone = assembler.append(respond, buffer);
        }

        debug << "[http] receive from \'" << host
            << "\', text length = " << respond.size() << std::endl;
        socket.close();
        return respond;
    }

    // methods
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

};  // class Requests

}  // namespace nanonet

#endif  // __REQUESTS_H__