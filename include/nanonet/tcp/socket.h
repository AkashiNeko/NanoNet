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
#include "nanonet/utility/addr_port.h"
#include "nanonet/exception/nanoerr.h"

namespace nanonet {

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
        this->connect(addrPort.getAddr(), addrPort.getPort());
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

} // namespace nanonet

#endif // __SOCKET_H__