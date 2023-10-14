// server_socket.hpp

#pragma once
#ifndef __SERVER_SOCKET_HPP__
#define __SERVER_SOCKET_HPP__

// nanonet
#include "utility/addr_port.hpp"
#include "tcp/tcp_socket.hpp"

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
    ServerSocket(const Addr& addr, const Port& port, int backlog = 20, bool setsockopt = true) {
        // socket
        serverSockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        assert(serverSockfd >= 0);

        // set socket option
        if (setsockopt) {
            const int on = 1;
            ::setsockopt(serverSockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        }

        // bind
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = addr.hton();
        local.sin_port = port.hton();
        assert(::bind(serverSockfd, (const struct sockaddr*)&local, sizeof(local)) >= 0);
        
        // listen
        assert(::listen(serverSockfd, backlog) >= 0);
    }
    
    ServerSocket(const std::string& ip, const Port& port, int backlog = 20, bool setsockopt = true)
        :ServerSocket(Addr(ip), port, backlog, setsockopt) {}

    ServerSocket(const char* ip, const Port& port, int backlog = 20, bool setsockopt = true)
        :ServerSocket(Addr(ip), port, backlog, setsockopt) {}

    ServerSocket(const Port& port, int backlog = 20, bool setsockopt = true)
        :ServerSocket(Addr(INADDR_ANY), port, backlog, setsockopt) {}


    // destructor
    ~ServerSocket() {
        // close server socket
        if (serverSockfd >= 0)
            this->close();
    }


    // accept from client
    TCPSocket accept() {
        TCPSocket socket; // result socket
        socklen_t socklen = sizeof(socket.remote);
        // accept
        socket.sockfd = ::accept(serverSockfd, (struct sockaddr*)&socket.remote, &socklen);
        assert(socket.sockfd >= 0);
        return socket;
    }


    // close server socket
    void close() {
        if (serverSockfd >= 0) {
            ::close(serverSockfd);
            serverSockfd = -1;
        }
    }

}; // class ServerSocket

} // namespace nanonet

#endif // __SERVER_SOCKET_HPP__