// server_socket.hpp

#pragma once
#ifndef __SERVER_SOCKET_HPP__
#define __SERVER_SOCKET_HPP__

// nanonet
#include "utility/addr_port.hpp"
#include "tcp/socket.hpp"

// linux
#include <unistd.h>

// C
#include <cassert>

// C++
#include <string>

namespace nanonet {

class ServerSocket {
    // server socket fd
    int serverSockfd;

    // local address
    struct sockaddr_in local;

public:
    // constructor (addr, port)
    ServerSocket(const Addr& addr, const Port& port, bool reuseAddr = true, int backlog = 30) {
        // socket
        serverSockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (serverSockfd < 0) {
            Log::error << "[tcp] socket: " << strerror(errno) << std::endl;
            exit(-1);
        }

        // set socket option
        if (reuseAddr) {
            const int on = 1;
            ::setsockopt(serverSockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        }

        // bind
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = addr.hton();
        local.sin_port = port.hton();
        if (::bind(serverSockfd, (const struct sockaddr*)&local, sizeof(local)) < 0) {
            Log::error << "[tcp] bind: " << strerror(errno) << std::endl;
            exit(-1);
        }

        // listen
        if (::listen(serverSockfd, backlog) < 0) {
            Log::error << "[tcp] listen: " << strerror(errno) << std::endl;
            exit(-1);
        } else {
            Log::debug << "[tcp] server listening to \'" << addr.toString() << ":"
                << port.toString() << '\'' << std::endl;
        }
    }

    ServerSocket(const std::string& ip, const Port& port, bool reuseAddr = true, int backlog = 100)
        : ServerSocket(Addr(ip), port, reuseAddr, backlog) {}

    ServerSocket(const char* ip, const Port& port, bool reuseAddr = true, int backlog = 100)
        : ServerSocket(Addr(ip), port, reuseAddr, backlog) {}

    ServerSocket(const Port& port, bool reuseAddr = true, int backlog = 100)
        : ServerSocket(Addr(INADDR_ANY), port, reuseAddr, backlog) {}

    // destructor
    ~ServerSocket() {}

    // accept from client
    Socket accept() {
        Socket socket;  // result socket
        socklen_t socklen = sizeof(socket.remote);
        // accept
        socket.sockfd = ::accept(serverSockfd, (struct sockaddr*)&socket.remote, &socklen);
        if (socket.sockfd < 0) {
            Log::error << "[tcp] accept: " << strerror(errno) << std::endl;
            exit(-1);
        }
        return socket;
    }

    // close server socket
    void close() {
        if (serverSockfd >= 0) {
            ::close(serverSockfd);
            serverSockfd = -1;
        } else {
            Log::warn << "[tcp] call close() repeatedly" << std::endl;
        }
    }

};  // class ServerSocket

}  // namespace nanonet

#endif  // __SERVER_SOCKET_HPP__