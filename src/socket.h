// socket.h

#pragma once
#ifndef __SOCKET_H__
#define __SOCKET_H__

// linux
#include <unistd.h>

// C
#include <cassert>

// C++
#include <string>

// nanonet
#include "addr_port.h"
#include "except.h"

namespace nano {

class Socket {
    
    // socket fd
    int sockfd = -1;

    // remote
    struct sockaddr_in remote;

    // server socket
    friend class ServerSocket;

public:

    // default constructor
    Socket();


    // destructor
    ~Socket();


    // connect to server
    void connect(const Addr& addr, const Port& port);

    inline void connect(const AddrPort& addrPort) {
        this->connect(addrPort.get_addr(), addrPort.get_port());
    }

    inline void connect(const std::string& ip, const Port& port) {
        this->connect(Addr(ip), port);
    }

    inline void connect(const char* ip, const Port& port) {
        this->connect(Addr(ip), port);
    }

    void bind(const Addr& addr, const Port& port);

    // close socket
    void close();

    inline bool isClosed() const {
        return sockfd == -1;
    }

    int setSocketOption(int level, int optname, const void* optval, socklen_t optlen);

    int setReceiveTimeout(long seconds, long milliseconds = 0);

    // send to remote
    int send(const char* msg, size_t size) const;

    int send(std::string msg) const;

    // receive from remote
    int receive(char *buf, size_t buf_size);

    // get remote addrport
    AddrPort getRemoteAddrPort() const;

}; // class Socket

} // namespace nano

#endif // __SOCKET_H__