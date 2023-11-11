// web_server.hpp

#pragma once
#ifndef __WEB_SERVER_HPP__
#define __WEB_SERVER_HPP__

#include <signal.h>

#include "utility/addr_port.hpp"
#include "tcp/socket.hpp"
#include "tcp/server_socket.hpp"
#include "http/http_request.hpp"
#include "http/http_respond.hpp"
#include "http/http_assembler.hpp"

#include <string>
#include <unordered_map>
#include <functional>

namespace nanonet {

using RouteMethod = std::function<void(const HttpRequest& request, HttpRespond& respond)>;

class WebServer {

    ServerSocket server;

    std::unordered_map<std::string, RouteMethod> routes;


    void fillRespond(const HttpRequest& request, HttpRespond& respond) {
        auto it = routes.find(request.getPath());
        if (it == routes.end()) {
            respond.setStatusCode(404);
            respond.setStatusMessage("Not Found");
            respond.setText("404 Note Found - nanonet");
        } else {
            auto& callback = it->second;
            callback(request, respond);
        }
    }

public:

    WebServer(const Addr& addr, const Port& port) :server(addr, port) {
        ::signal(SIGCHLD, SIG_IGN);
        Log::info << "[http] web serber listening to "
            << addr.toString() << " port "
            << port.toString() << std::endl;
    }

    WebServer(const Port& port) :WebServer(INADDR_ANY, port) {}
    
    void route(const std::string& path, RouteMethod todo) {
        routes[path] = todo;
    }


    int exec() {
        while (true) {
            Socket socket = server.accept();
            if (fork()) {
                socket.close();
            } else {
                server.close();
                socket.setReceiveTimeout(10);
                const int BUF_SIZE = 4096;
                char buffer[BUF_SIZE]{};

                // receive
                int receiveLength = socket.receive(buffer, BUF_SIZE - 1);

                if (receiveLength < 0) {
                    // receive failed
                    Log::warn << "[http] receive: " << strerror(errno) << std::endl;
                    socket.close();
                    return {};
                } else if (receiveLength == 0) {
                    // server not responding
                    Log::warn << "[http] server not responding" << std::endl;
                    socket.close();
                    return {};
                }
                buffer[receiveLength] = '\0';

                HttpRequest request;
                HttpAssembler assembler;
                bool receiveDone = assembler.append(request, buffer);
                while (!receiveDone && receiveLength > 0) {
                    socket.setReceiveTimeout(1, 0);
                    buffer[0] = '\0';
                    receiveLength = socket.receive(buffer, BUF_SIZE - 1);
                    if (receiveLength >= 0)
                        buffer[receiveLength] = '\0';
                    receiveDone = assembler.append(request, buffer);
                }
                HttpRespond respond;
                fillRespond(request, respond);
                Log::info << "[http] receive from " << socket.getRemoteAddrPort().toString()
                    << " - " << request.getPath() << std::endl;
                socket.send(respond.toString());
                socket.close();
                return 0;
            }
        }
        return 0;
    }

}; // class WebServer

} // namespace nanonet

#endif // __WEB_SERVER_HPP__