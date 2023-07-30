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
    struct sockaddr_in target_{};

public:

    // constructor (not bind)
    UdpSocket()
        : sockfd_(-1), local_({}), target_({}) {
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
    ~UdpSocket() {
        this->close();
    }

    // set target
    void target(AddrPort addrPort) {
        assert(sockfd_ >= 0);
        // set ip and port of the target
        target_.sin_family = AF_INET;
        target_.sin_port = addrPort.getNetPort();
        target_.sin_addr.s_addr = addrPort.getNetAddr();
    }

    // set target from address and port
    void target(std::string ip, port_t port) {
        this->target({ip, port});
    }

    // send data
    int send(UdpPacket packet) {
        assert(sockfd_ >= 0);
        std::string ip = inet_ntoa(target_.sin_addr);
        auto port = ntohs(target_.sin_port);
        return (int)sendto(sockfd_, packet.data(), packet.size(),
            0, (const struct sockaddr*)&target_, sizeof(target_));
    }

    int send(const char* data, size_t datalen) {
        return this->send(UdpPacket(data, datalen));
    }
    
    int send(const std::string str) {
        return this->send(UdpPacket(str.c_str(), str.size()));
    }

    // waiting to receive data from others
    UdpPacket receive() {
        assert(sockfd_ >= 0);
        char buf[4096];
        struct sockaddr_in remote{};
        socklen_t socklen = sizeof(remote);
        ssize_t len = recvfrom(sockfd_, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&remote, &socklen);
        assert(len >= 0); // received successfully
        buf[len] = '\0';
        AddrPort addrPort(remote.sin_addr.s_addr, remote.sin_port);
        UdpPacket packet(buf, len);
        packet.setAddrPort(addrPort);
        return packet;
    }

    // close socket fd
    void close() {
        if (sockfd_ >= 0) {
            ::close(sockfd_);
            sockfd_ = -1;
        }
    }

}; // class UdpSocket

} // namespace nanonet

#endif // __UDP_SOCKET_HPP__