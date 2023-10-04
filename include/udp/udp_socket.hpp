// udp_socket.hpp

#pragma once
#ifndef __UDP_SOCKET_HPP__
#define __UDP_SOCKET_HPP__

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

class udp_socket {

    // socket fd
    int sockfd_;

    // local address
    struct sockaddr_in local_{};

    // target address
    struct sockaddr_in remote_{};


public:

    // constructor (not bind)
    udp_socket() :sockfd_(-1), local_({}), remote_({}) {
        local_.sin_family = AF_INET;
        sockfd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
        assert(sockfd_ >= 0);
    }


    // constructor (bind port)
    udp_socket(port_t port) :udp_socket() {
        local_.sin_addr.s_addr = INADDR_ANY; // any ip address
        local_.sin_port = htons(port);
        int bind_ret = bind(sockfd_, (const sockaddr*)&local_, sizeof(local_));
        assert(bind_ret >= 0);
    }


    // constructor (bind ip and port)
    udp_socket(std::string ip, port_t port) :udp_socket() {
        in_addr_t addr = local_.sin_addr.s_addr = inet_addr(ip.c_str());
        assert(addr != INADDR_NONE);
        local_.sin_port = htons(port);
        int bind_ret = bind(sockfd_, (const sockaddr*)&local_, sizeof(local_));
        assert(bind_ret >= 0);
    }


    // destructor (close socket fd)
    virtual ~udp_socket() {
        this->close();
    }


    // set target
    inline void remote(addr_port addrPort) {
        assert(sockfd_ >= 0);
        // set ip and port of the target
        remote_.sin_family = AF_INET;
        remote_.sin_port = addrPort.net_port();
        remote_.sin_addr.s_addr = addrPort.net_addr();
    }


    // set target from address and port
    inline void remote(std::string ip, port_t port) {
        // ip, port -> addr_port
        this->remote({ip, port});
    }


    // send data
    inline int send(const char* data, size_t datalen) {
        assert(sockfd_ >= 0);
        return (int)::sendto(sockfd_, data, datalen, 0,
            (const struct sockaddr*)&remote_, sizeof(remote_));
    }

    inline int send(const std::string str) {
        // string -> const char*
        return this->send(str.c_str(), str.size());
    }

    inline int sendto(addr_port addrPort, const char* data, size_t datalen) {
        assert(sockfd_ >= 0);
        this->remote(addrPort);
        return (int)::sendto(sockfd_, data, datalen, 0,
            (const struct sockaddr*)&addrPort, sizeof(addrPort));
    }


    // waiting to receive data from others
    inline addr_port receive(char* buf, size_t buf_size) {
        assert(sockfd_ >= 0);
        socklen_t socklen = sizeof(remote_);

        // receive data & write to buffer
        ssize_t len = ::recvfrom(sockfd_, buf, buf_size, 0, (struct sockaddr*)&remote_, &socklen);
        
        // received successfully
        assert(len >= 0);

        // truncate buffer
        if (len < buf_size)
            buf[len] = '\0';
        else
            buf[buf_size - 1] = '\0';

        // return ip and port
        return addr_port(remote_.sin_addr.s_addr, remote_.sin_port);
    }


    // close socket fd
    inline void close() {
        if (sockfd_ >= 0) {
            ::close(sockfd_);
            sockfd_ = -1;
        }
    }

}; // class udp_socket

} // namespace nanonet

#endif // __UDP_SOCKET_HPP__
