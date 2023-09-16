// server_socket.hpp

#pragma once
#ifndef __SERVER_SOCKET_HPP__
#define __SERVER_SOCKET_HPP__

// nanonet
#include "nanonet/nano_def.hpp"
#include "nanonet/addr_port.hpp"
#include "nanonet/socket.hpp"

// linux
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// C
#include <cassert>

// C++
#include <string>

namespace nanonet {

class server_socket {
    
    // ServerSocket fd
    int server_fd_;
    // local address
    struct sockaddr_in local_;

private:

    // init object
    inline void init(const char* ip, port_t port, bool setsockopt_) {
        // socket
        server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        assert(server_fd_ >= 0);

        // start in the TIME_WAIT state
        if (setsockopt_) {
            const int on = 1;
            setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        }
        
        // bind
        local_.sin_family = AF_INET;
        local_.sin_addr.s_addr = ip ? inet_addr(ip) : INADDR_ANY;
        local_.sin_port = htons(port);
        assert(bind(server_fd_, (const struct sockaddr*)&local_, sizeof(local_)) >= 0);
        
        // listen
        assert(listen(server_fd_, 20) >= 0);
    }

public:

    // constructor (port)
    server_socket(port_t port, bool setsockopt = true) {
        // any ip address (0.0.0.0)
        this->init(nullptr, port, setsockopt);
    }

    // constructor (ip, port)
    server_socket(std::string ip, port_t port, bool setsockopt = true) {
        this->init(ip.c_str(), port, setsockopt);
    }

    // destructor
    ~server_socket() {
        // close server socket
        if (server_fd_ >= 0)
            this->close();
    }

    // accept from client
    Socket accept() const {
        Socket socket; // result socket
        socklen_t socklen = sizeof(socket.remote_);
        // accept
        socket.sockfd_ = ::accept(server_fd_, (struct sockaddr*)&socket.remote_, &socklen);
        assert(socket.sockfd_ >= 0);
        return socket;
    }

    // close server socket
    void close() {
        if (server_fd_ >= 0) {
            ::close(server_fd_);
            server_fd_ = -1;
        }
    }

}; // class ServerSocket

} // namespace nanonet

#endif // __SERVER_SOCKET_HPP__