// socket.h

#pragma once
#ifndef NANONET_SOCKET_H_
#define NANONET_SOCKET_H_

// nanonet
#include "socket_base.h"

namespace nano {

class Socket : public SocketBase {

    // remote address
    sockaddr_in remote_;

    // server socket
    friend class ServerSocket;

public:

    // ctor & dtor
    Socket();
    virtual ~Socket() = default;

    // connect to remote
    void connect(const Addr& addr, const Port& port);

    // send to remote
    int send(const char* msg, size_t length) const;
    int send(const std::string msg) const;

    // receive from remote
    int receive(char *buf, size_t buf_size) const;

    // set receive timeout
    bool recv_timeout(long ms) const;

    // get remote
    AddrPort get_remote() const;

}; // class Socket

} // namespace nano

#endif // NANONET_SOCKET_H_
