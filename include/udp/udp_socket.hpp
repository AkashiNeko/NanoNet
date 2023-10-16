// udp_socket.hpp

#pragma once
#ifndef __UDP_SOCKET_HPP__
#define __UDP_SOCKET_HPP__

// nanonet
#include "utility/addr_port.hpp"

// linux
#include <sys/socket.h>
#include <unistd.h>

// C
#include <cassert>

// C++
#include <string>

namespace nanonet {

class UDPSocket {

    // socket fd
    int sockfd;

    // local address
    struct sockaddr_in local;

    // remote address
    struct sockaddr_in remote;


public:

    // default constructor
    UDPSocket() :sockfd(-1), local({}), remote({}) {
        local.sin_family = AF_INET;
        sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
        assert(sockfd >= 0);
    }


    // destructor
    ~UDPSocket() {
        this->close();
    }


    // bind (addr : port)
    inline int bind(const Addr& addr, const Port& port) {
        local.sin_addr.s_addr = addr.hton();
        local.sin_port = port.hton();
        int bindResult = ::bind(sockfd, (const sockaddr*)&local, sizeof(local));
        return bindResult;
    }

    inline int bind(const AddrPort& addrPort) {
        return this->bind(addrPort.addr, addrPort.port);
    }

    inline int bind(const std::string& ip, const Port& port) {
        return this->bind(Addr(ip), port);
    }

    inline int bind(const char* ip, const Port& port) {
        return this->bind(Addr(ip), port);
    }

    inline int bind(const Port& port) {
        return this->bind(Addr(INADDR_ANY), port);
    }


    // set remote address & port
    inline void setRemote(const Addr& addr, const Port& port) {
        assert(sockfd >= 0);
        remote.sin_family = AF_INET;
        remote.sin_port = port.hton();
        remote.sin_addr.s_addr = addr.hton();
    }

    inline void setRemote(const AddrPort& addrPort) {
        this->setRemote(addrPort.addr, addrPort.port);
    }

    inline void setRemote(const std::string& ip, const Port& port) {
        this->setRemote(Addr(ip), port);
    }

    inline void setRemote(const char* ip, const Port& port) {
        this->setRemote(Addr(ip), port);
    }


    // send data
    inline int send(const char* data, size_t datalen) {
        assert(sockfd >= 0);
        return (int)::sendto(sockfd, data, datalen, 0,
            (const struct sockaddr*)&remote, sizeof(remote));
    }

    inline int send(const std::string& str) {
        return this->send(str.c_str(), str.size());
    }


    // waiting to receive data from others
    inline AddrPort receive(char* buffer, size_t bufSize) const {
        assert(sockfd >= 0);
        socklen_t socklen = sizeof(remote);

        // receive data & write to buffer
        ssize_t len = ::recvfrom(sockfd, buffer, bufSize, 0, (struct sockaddr*)&remote, &socklen);
        
        // received successfully
        assert(len >= 0);

        // truncate buffer
        if (len < bufSize) buffer[len] = 0;

        // return ip and port
        return AddrPort(::ntohl(remote.sin_addr.s_addr), ::ntohs(remote.sin_port));
    }


    // close socket fd
    inline void close() {
        if (sockfd >= 0) {
            ::close(sockfd);
            sockfd = -1;
        }
    }

}; // class UDPSocket

} // namespace nanonet

#endif // __UDP_SOCKET_HPP__
