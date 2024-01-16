// socket_base.h

#pragma once
#ifndef NANONET_SOCKET_BASE_H
#define NANONET_SOCKET_BASE_H

// Linux
#include <unistd.h>

// nanonet
#include "addr_port.h"

namespace nano {

#ifdef __WIN32__ // Windows
    using sock_t = SOCKET;
#elif __linux__ // Linux
    using sock_t = int;
#else // other
    #error "Unsupported platform. Only Windows and Linux are supported."
#endif // platform

class SocketBase {
protected:

    // fd of socket
    sock_t sock_fd_;

    // local address
    sockaddr_in local_;

public:

    // ctor & dtor
    SocketBase(int fd = -1);
    virtual ~SocketBase() = default;

    // file descriptor
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
        return ::setsockopt(sock_fd_, level, optname, &optval, sizeof(optval)) == 0;
    }

    template <class OptionType>
    inline bool get_option(int level, int optname, OptionType& optval) const {
        return ::getsockopt(sock_fd_, level, optname, &optval, sizeof(optval)) == 0;
    }

}; // class SocketBase

} // namespace nano

#endif //NANONET_SOCKET_BASE_H
