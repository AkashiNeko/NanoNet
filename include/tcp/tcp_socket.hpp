// tcp_socket.hpp

#pragma once
#ifndef __TCP_SOCKET_HPP__
#define __TCP_SOCKET_HPP__

// nanonet
#include "utility/nano_def.hpp"
#include "utility/addr_port.hpp"

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

class tcp_socket {
    
    // socket fd
    int sockfd_;

    // remote address
    struct sockaddr_in remote_;

    // server socket (when build socket object)
    friend class server_socket;

public:

    // default constructor
    tcp_socket() {
        remote_.sin_family = AF_INET;
    }


    // client constructor
    tcp_socket(std::string ip, int port) {
        sockfd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        remote_.sin_family = AF_INET;
        remote_.sin_addr.s_addr = ::inet_addr(ip.c_str());
        remote_.sin_port = ::htons(port);
    }


    // destructor (close fd)
    ~tcp_socket() {
        if (sockfd_ >= 0)
            this->close();
    }


    // receive from remote
    inline size_t receive(char *buf, size_t buf_size) {
        // assert
        assert(sockfd_ >= 0);
        assert(buf_size > 0);

        // receive from remote
        size_t ret = ::recv(sockfd_, buf, buf_size, 0);
        assert(ret >= 0);

        // truncate buffer
        if (ret < buf_size)
            buf[ret] = '\0';
        else
            buf[buf_size - 1] = '\0';

        // returns the number of bytes receive
        return (size_t)ret;
    }


    // send to remote (C string)
    inline ssize_t send(const char* msg, size_t size) {
        assert(sockfd_ >= 0);

        // send to remote
        ssize_t ret = ::send(sockfd_, msg, size, 0);
        assert(ret >= 0);

        // returns the number of bytes sent
        return ret;
    }


    // send to remote (C++ string)
    inline ssize_t send(std::string msg) {
        return this->send(msg.c_str(), msg.size());
    }


    // connect to server
    inline void connect() {
        assert(::connect(sockfd_, (const struct sockaddr*)&remote_, sizeof(remote_)) >= 0);
    }


    // close socket
    inline void close() {
        if (sockfd_ >= 0) {
            ::close(sockfd_);
            sockfd_ = -1;
        }
    }


    // get remote address (uint32_t)
    inline addr_t remote_addr() {
        return ::ntohl(remote_.sin_addr.s_addr);
    }


    // get remote port
    inline port_t remote_port() {
        return ::ntohs(remote_.sin_port);
    }


    // get remote addr_port
    inline addr_port remote_addrport() {
        return {remote_.sin_addr.s_addr, remote_.sin_port};
    }

}; // class socket

} // namespace nanonet

#endif // __SOCKET_HPP__