// udp_socket.h

#pragma once
#ifndef NANONET_UDP_SOCKET_H
#define NANONET_UDP_SOCKET_H

// nanonet
#include "socket_base.h"

namespace nano {

class UdpSocket : public SocketBase {

    // remote address
    sockaddr_in remote_;

    // is connected
    bool is_connected_;

public:

    // ctor & dtor
    UdpSocket();
    virtual ~UdpSocket() = default;

    // send to the specified remote
    int send_to(const char* msg, size_t length, const AddrPort& remote) const;
    int send_to(const std::string& msg, const AddrPort& remote) const;

    // receive from the specified remote
    int receive_from(char* buf, size_t buf_size, AddrPort& addrport) const;
    int receive_from(char* buf, size_t buf_size) const;

    // connect to remote
    void connect(const Addr& addr, const Port& port);

    // send & receive
    int send(const char* msg, size_t length) const;
    int send(const std::string& msg) const;
    int receive(char* buf, size_t buf_size) const;

    // set receive timeout
    bool recv_timeout(long ms) const;

    // getter
    AddrPort get_remote() const;

}; // class UdpSocket

} // namespace nano

#endif // NANONET_UDP_SOCKET_H
