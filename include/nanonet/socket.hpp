// socket.hpp

#pragma once
#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

// nanonet
#include "nanonet/nano_def.hpp"
#include "nanonet/addr_port.hpp"

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

class Socket {
    
    int sockfd_;
    struct sockaddr_in remote_;

    friend class ServerSocket;

public:

    Socket() {
        remote_.sin_family = AF_INET;
    }

    Socket(std::string ip, int port) {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        remote_.sin_family = AF_INET;
        remote_.sin_addr.s_addr = inet_addr(ip.c_str());
        remote_.sin_port = htons(port);
    }

    ~Socket() {
        if (sockfd_ >= 0)
            this->close();
    }

    inline size_t receive(char *buf, size_t maxSize) {
        assert(sockfd_ >= 0);
        size_t ret = recv(sockfd_, buf, maxSize, 0);
        buf[ret] = '\0';
        assert(ret >= 0);
        return (size_t)ret;
    }

    inline ssize_t send(const char* msg, size_t size) {
        assert(sockfd_ >= 0);
        ssize_t ret = ::send(sockfd_, msg, size, 0);
        assert(ret >= 0);
        return ret;
    }

    inline ssize_t send(std::string msg) {
        return this->send(msg.c_str(), msg.size());
    }

    inline void connect() {
        assert(::connect(sockfd_, (const struct sockaddr*)&remote_, sizeof(remote_)) >= 0);
    }

    inline void close() {
        if (sockfd_ >= 0) {
            ::close(sockfd_);
            sockfd_ = -1;
        }
    }

    // get remote address (uint32_t)
    inline addr_t getRemoteAddr() {
        return ntohl(remote_.sin_addr.s_addr);
    }

    // get string remote address (xxx.xxx.xxx.xxx)
    inline std::string getRemoteStrAddr() {
        char buffer[INET_ADDRSTRLEN];
        const char* result = inet_ntop(AF_INET, &(remote_.sin_addr.s_addr), buffer, sizeof(remote_));
        return result == nullptr ? std::string() : buffer;
    }

    // get remote port
    inline port_t getRemotePort() {
        return ntohs(remote_.sin_port);
    }

}; // class Socket

} // namespace nanonet

#endif // __SOCKET_HPP__