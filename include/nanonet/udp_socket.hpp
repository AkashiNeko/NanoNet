// udp_socket.hpp

#pragma once
#ifndef __UDP_SOCKET_HPP__
#define __UDP_SOCKET_HPP__

// nanonet
#include "nanonet/nano_def.hpp"
#include "nanonet/addr_port.hpp"
#include "nanonet/udp_packet.hpp"

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

class UdpSocket {

    // socket fd
    int sockfd_;
    // local address
    struct sockaddr_in local_{};
    // target address
    struct sockaddr_in remote_{};

public:

    // constructor (not bind)
    UdpSocket()
        : sockfd_(-1), local_({}), remote_({}) {
        local_.sin_family = AF_INET;
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        assert(sockfd_ >= 0);
    }

    // constructor (bind port)
    UdpSocket(port_t port) :UdpSocket() {
        local_.sin_addr.s_addr = INADDR_ANY; // any ip address
        local_.sin_port = htons(port);
        int bind_ret = bind(sockfd_, (const sockaddr*)&local_, sizeof(local_));
        assert(bind_ret >= 0);
    }

    // constructor (bind ip and port)
    UdpSocket(std::string ip, port_t port) :UdpSocket() {
        in_addr_t addr = local_.sin_addr.s_addr = inet_addr(ip.c_str());
        assert(addr != INADDR_NONE);
        local_.sin_port = htons(port);
        int bind_ret = bind(sockfd_, (const sockaddr*)&local_, sizeof(local_));
        assert(bind_ret >= 0);
    }

    // destructor (close socket fd)
    virtual ~UdpSocket() {
        this->close();
    }

    // set target
    inline void remote(AddrPort addrPort) {
        assert(sockfd_ >= 0);
        // set ip and port of the target
        remote_.sin_family = AF_INET;
        remote_.sin_port = addrPort.getNetPort();
        remote_.sin_addr.s_addr = addrPort.getNetAddr();
    }

    // set target from address and port
    inline void remote(std::string ip, port_t port) {
        this->remote({ip, port});
    }

    // send data
    inline int send(const char* data, size_t datalen) {
        assert(sockfd_ >= 0);
        return (int)::sendto(sockfd_, data, datalen, 0,
            (const struct sockaddr*)&remote_, sizeof(remote_));
    }
    
    inline int send(const std::string str) {
        return this->send(str.c_str(), str.size());
    }

    inline int sendto(AddrPort addrPort, const char* data, size_t datalen) {
        assert(sockfd_ >= 0);
        this->remote(addrPort);
        return (int)::sendto(sockfd_, data, datalen, 0,
            (const struct sockaddr*)&addrPort, sizeof(addrPort));
    }

    // waiting to receive data from others
    inline UdpPacket receive(char* buf, size_t bufSize) {
        assert(sockfd_ >= 0);
        socklen_t socklen = sizeof(remote_);

        // receive data
        ssize_t len = recvfrom(sockfd_, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&remote_, &socklen);
        assert(len >= 0); // received successfully
        if (len < bufSize)
            buf[len] = '\0';
        // build packet
        UdpPacket packet(buf, len);
        AddrPort addrPort(remote_.sin_addr.s_addr, remote_.sin_port);
        packet.setAddrPort(addrPort);
        return packet;
    }

    // close socket fd
    inline void close() {
        if (sockfd_ >= 0) {
            ::close(sockfd_);
            sockfd_ = -1;
        }
    }

}; // class UdpSocket

} // namespace nanonet

#endif // __UDP_SOCKET_HPP__