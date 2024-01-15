// server_socket.h

#pragma once
#ifndef __SERVER_SOCKET_H__
#define __SERVER_SOCKET_H__

// C
#include <cassert>

// C++
#include <string>

// Linux
#include <unistd.h>

// nanonet
#include "addr_port.h"
#include "socket.h"
#include "socket_base.h"
#include "except.h"

namespace nano {

const int BACKLOG_QUEUE_SIZE = 20;

class ServerSocket : public SocketBase {

    // local address
    struct sockaddr_in local_;

    // is listening
    bool listening = false;

public:
    // constructor (addr, port)
    ServerSocket();

    ServerSocket(const Addr& addr, const Port& port);

    // destructor
    virtual ~ServerSocket();

    void reuse_addr(bool reuseAddr);

    void bind(const Addr& addr, const Port& port);

    void listen(int backlog = 20);

    // accept from client
    Socket accept();

};  // class ServerSocket

}  // namespace nano

#endif  // __SERVER_SOCKET_H__
