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
    UdpSocket();


    // destructor
    ~UdpSocket();


    // bind (addr : port)
    void bind(const Addr& addr, const Port& port);

    void bind(const AddrPort& addrPort);

    void bind(const std::string& ip, const Port& port);

    void bind(const char* ip, const Port& port);

    void bind(const Port& port);


    // set remote address & port
    void setRemote(const Addr& addr, const Port& port);

    void setRemote(const AddrPort& addrPort);

    void setRemote(const std::string& ip, const Port& port);

    void setRemote(const char* ip, const Port& port);

    int setReceiveTimeout(long seconds, long milliseconds = 0);

    // send data
    int send(const char* data, size_t datalen) const;

    int send(const std::string& str) const;

    // waiting to receive data from others
    AddrPort receive(char* buffer, size_t bufSize) const;

    // close socket fd
    void close();

    int setsockopt(int level, int optname, const void* optval, socklen_t optlen) const;

}; // class UdpSocket

} // namespace nanonet

#endif // __UDP_SOCKET_HPP__
