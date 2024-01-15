// server_socket.h

#pragma once
#ifndef __SERVER_SOCKET_H__
#define __SERVER_SOCKET_H__

// nanonet
#include "socket.h"

namespace nano {

class ServerSocket : public SocketBase {
public:

    // ctor & dtor
    ServerSocket();
    ServerSocket(const Addr& addr, const Port& port);
    virtual ~ServerSocket() = default;

    // listen
    void listen(int backlog = 20);

    // accept from client
    Socket accept();

    // set address reuse
    void reuse_addr(bool reuseAddr);

}; // class ServerSocket

} // namespace nano

#endif // __SERVER_SOCKET_H__
