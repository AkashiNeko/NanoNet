// udp_socket.h

#pragma once
#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__

// nanonet
#include "addr_port.h"
#include "except.h"

// linux
#include <sys/socket.h>
#include <unistd.h>

// C
#include <cassert>

// C++
#include <string>

namespace nano {

class UdpSocket {

    // socket fd
    int sockfd;

    // local address
    struct sockaddr_in local;

    // remote address
    struct sockaddr_in remote;


public:

    // default constructor
    UdpSocket();


    // destructor
    ~UdpSocket();


    // bind (addr : port)
    void bind(const Addr& addr, const Port& port);

    inline void bind(const AddrPort& addrPort) {
        this->bind(addrPort.get_addr(), addrPort.get_port());
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
    void setRemote(const Addr& addr, const Port& port);

    inline void setRemote(const AddrPort& addrPort) {
        this->setRemote(addrPort.get_addr(), addrPort.get_port());
    }

    inline void setRemote(const std::string& ip, const Port& port) {
        this->setRemote(Addr(ip), port);
    }

    inline void setRemote(const char* ip, const Port& port) {
        this->setRemote(Addr(ip), port);
    }

    int setReceiveTimeout(long seconds, long milliseconds = 0);

    // send data
    int send(const char* data, size_t datalen) const;

    inline int send(const std::string& str) const {
        return this->send(str.c_str(), str.size());
    }

    // waiting to receive data from others
    AddrPort receive(char* buffer, size_t bufSize) const;

    // close socket fd
    void close();

    int setsockopt(int level, int optname, const void* optval, socklen_t optlen) const;

}; // class UdpSocket

} // namespace nano

#endif // __UDP_SOCKET_H__