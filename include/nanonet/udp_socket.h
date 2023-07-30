// udp_socket.h

#pragma once
#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__

#include "nanonet/nano_def.h"
#include "nanonet/addr_port.h"
#include "nanonet/udp_packet.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cassert>

namespace nanonet {

class UdpSocket {
    int sockfd_;
    struct sockaddr_in local_{};
    struct sockaddr_in target_{};

public:
    UdpSocket();
    UdpSocket(port_t port);

    // ~UdpSocket();

    // set target
    void target(AddrPort addrPort);
    // set target from address and port
    inline void target(std::string ip, port_t port) {
        this->target({ip, port});
    }

    // send data
    int send(UdpPacket packet);
    
    inline int send(const char* data, size_t datalen = -1) {
        return this->send(UdpPacket(data, datalen));
    }
    
    inline int send(const std::string str) {
        return this->send(UdpPacket(str.c_str(), str.size()));
    }

    UdpPacket receive();

}; // class UdpSocket

} // namespace nanonet

#endif // __UDP_SOCKET_H__