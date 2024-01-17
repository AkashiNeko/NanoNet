// socket_base.h

#pragma once
#ifndef NANONET_SOCKET_BASE_H
#define NANONET_SOCKET_BASE_H

#ifdef __linux__

#include <unistd.h>

#endif

// nanonet
#include "addr_port.h"

namespace nano {

#ifdef __linux__
    using sock_t = int;
#elif _WIN32
    using sock_t = SOCKET;
#else
    #error "Unsupported platform. Only Windows and Linux are supported."
#endif

class SocketBase {
protected:

    // linux: fd, windows: SOCKET
    sock_t socket_;

#ifdef _WIN32
    bool sock_open_;
#endif

    // local address
    sockaddr_in local_;

protected:

    // create a socket
    void create_socket_(int type);

    // ctor
    SocketBase();

public:
    // dtor
    virtual ~SocketBase() = default;

    // socket
    virtual void close();
    virtual bool is_open() const;
    virtual sock_t get_sock() const;

    // bind local
    void bind(const Addr& addr, const Port& port);

    // get local
    AddrPort get_local() const;

    // set socket option
    template <class OptionType>
    inline bool set_option(int level, int optname, const OptionType& optval) const {
        return ::setsockopt(socket_, level, optname, (const char*)&optval, sizeof(optval)) == 0;
    }

    template <class OptionType>
    inline bool get_option(int level, int optname, OptionType& optval) const {
        return ::getsockopt(socket_, level, optname, (const char*)&optval, sizeof(optval)) == 0;
    }

}; // class SocketBase

} // namespace nano

#endif //NANONET_SOCKET_BASE_H
