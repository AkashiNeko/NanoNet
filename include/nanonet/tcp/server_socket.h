// server_socket.h

#pragma once
#ifndef __SERVER_SOCKET_H__
#define __SERVER_SOCKET_H__

// nanonet
#include "nanonet/utility/addr_port.h"
#include "nanonet/tcp/socket.h"
#include "nanonet/exception/nanoerr.h"

// linux
#include <unistd.h>

// C
#include <cassert>

// C++
#include <string>

namespace nanonet {

const int BACKLOG_QUEUE_SIZE = 20;

class ServerSocket {
    // server socket fd
    int serverfd = -1;

    // local address
    struct sockaddr_in local;

    // is listening
    bool listening = false;

public:
    // constructor (addr, port)
    ServerSocket();

    ServerSocket(const Addr& addr, const Port& port);

    inline ServerSocket(const std::string& ip, const Port& port)
        :ServerSocket(Addr(ip), port) {}

    inline ServerSocket(const char* ip, const Port& port)
        :ServerSocket(Addr(ip), port) {}

    inline ServerSocket(const Port& port)
        :ServerSocket(Addr(INADDR_ANY), port) {}

    // destructor
    virtual ~ServerSocket();

    void setReuseAddr(bool reuseAddr);

    void bind(const Addr& addr, const Port& port);

    inline void bind(const std::string& ip, const Port& port) {
        this->bind(Addr(ip), port);
    }

    inline void bind(const char* ip, const Port& port) {
        this->bind(Addr(ip), port);
    }

    inline void bind(const Port& port) {
        this->bind(Addr(INADDR_ANY), port);
    }

    void listen(int backlog = 20);

    // accept from client
    Socket accept();

    // close server socket
    void close();

};  // class ServerSocket

}  // namespace nanonet

#endif  // __SERVER_SOCKET_H__
