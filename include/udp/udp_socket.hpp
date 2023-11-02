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

class UdpSocket {

    // socket fd
    int sockfd;

    // local address
    struct sockaddr_in local;

    // remote address
    struct sockaddr_in remote;


public:

    // default constructor
    UdpSocket() :sockfd(-1), local({}), remote({}) {
        local.sin_family = AF_INET;
        sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
        assert(sockfd >= 0);
    }


    // destructor
    ~UdpSocket() {}


    // bind (addr : port)
    inline void bind(const Addr& addr, const Port& port) {
        local.sin_addr.s_addr = addr.hton();
        local.sin_port = port.hton();
        if (::bind(sockfd, (const sockaddr*)&local, sizeof(local)) < 0) {
            Log::error << "[udp] bind: " << strerror(errno) << std::endl;
            exit(-1);
        }
    }

    inline void bind(const AddrPort& addrPort) {
        this->bind(addrPort.addr, addrPort.port);
    }

    inline void bind(const std::string& ip, const Port& port) {
        this->bind(Addr(ip), port);
    }

    inline void bind(const char* ip, const Port& port) {
        this->bind(Addr(ip), port);
    }

    inline void bind(const Port& port) {
        this->bind(Addr(INADDR_ANY), port);
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

    int setReceiveTimeout(long seconds, long milliseconds = 0) {
        struct timeval tm = {seconds, milliseconds * 1000};
        return this->setsockopt(SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(struct timeval));
    }

    // send data
    inline int send(const char* data, size_t datalen) const {
        if (sockfd < 0) {
            Log::error << "[udp] socket is closed" << std::endl;
            exit(-1);
        }
        ssize_t ret = ::sendto(sockfd, data, datalen, 0,
            (const struct sockaddr*)&remote, sizeof(remote));
        if (ret < 0) {
            Log::warn << "[udp] send: " << strerror(errno) << std::endl;
        } else if (ret == 0) {
            Log::warn << "[udp] send: no data was sent";
        }
        return (int)ret;
    }

    inline int send(const std::string& str) const {
        return this->send(str.c_str(), str.size());
    }


    // waiting to receive data from others
    inline AddrPort receive(char* buffer, size_t bufSize) const {
        if (sockfd < 0) {
            Log::error << "[udp] socket is closed" << std::endl;
            exit(-1);
        }

        // receive data & write to buffer
        socklen_t socklen = sizeof(remote);
        ssize_t len = ::recvfrom(sockfd, buffer, bufSize, 0, (struct sockaddr*)&remote, &socklen);
        
        // received successfully
        if (len < 0) {
            Log::warn << "[udp] receive: " << strerror(errno) << std::endl;
        } else if (len == 0) {
            Log::warn << "[udp] receive: no data was received";
        }

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
            Log::debug << "[udp] connection closed" << std::endl;
        } else {
            Log::warn << "[udp] call close() repeatedly" << std::endl;
        }
    }

    inline int setsockopt(int level, int optname, const void* optval, socklen_t optlen) const {
        return ::setsockopt(sockfd, level, optname, optval, optlen);
    }

}; // class UdpSocket

} // namespace nanonet

#endif // __UDP_SOCKET_HPP__
