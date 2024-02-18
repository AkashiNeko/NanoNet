// File:     src/SocketBase.h
// Author:   AkashiNeko
// Project:  NanoNet
// Github:   https://github.com/AkashiNeko/NanoNet/

/* Copyright AkashiNeko. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 *
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once
#ifndef NANONET_SOCKET_BASE_H
#define NANONET_SOCKET_BASE_H

// NanoNet
#include "AddrPort.h"

#ifdef NANO_LINUX

#include <unistd.h>
#include <fcntl.h>

#elif NANO_WINDOWS

#include <ws2tcpip.h>

#endif

#define NULL_SOCKET (-1)

namespace nano {

class SocketBase {
protected:

    sock_t socket_;

    // local address
    addr_t local_addr_;
    port_t local_port_;

protected:

    // ctor & dtor
    SocketBase(int type);
    virtual ~SocketBase() = default;

    // move
    SocketBase(SocketBase&& other) noexcept;
    SocketBase& operator=(SocketBase&& other) noexcept;

    // uncopyable
    SocketBase(const SocketBase&) = delete;
    SocketBase& operator=(const SocketBase&) = delete;

public:

    // socket
    void close() noexcept;
    bool is_open() const noexcept;
    sock_t get() const noexcept;

    // bind local
    void bind(const Addr& addr, const Port& port);
    void bind(const AddrPort& addrport);

    // get local
    AddrPort local() const noexcept;

    // blocking
    bool set_blocking(bool blocking);

    // socket option
    template <class Ty>
    inline bool set_option(int level, int optname, const Ty& optval) const {
#ifdef NANO_LINUX
        using const_arg_t = const void*;
#elif NANO_WINDOWS
        using const_arg_t = const char*;
#endif
        return 0 == ::setsockopt(socket_, level, optname,
            (const_arg_t)&optval, sizeof(optval));
    }

    template <class Ty>
    inline bool get_option(int level, int optname, Ty& optval) const {
        socklen_t socklen = sizeof(optval);
#ifdef NANO_LINUX
        using arg_t = void*;
#elif NANO_WINDOWS
        using arg_t = char*;
#endif
        return ::getsockopt(socket_, level, optname,
            (arg_t)&optval, &socklen) == 0;
    }

protected:
    virtual const char* except_name() const noexcept;

}; // class SocketBase

} // namespace nano

#endif // NANONET_SOCKET_BASE_H
