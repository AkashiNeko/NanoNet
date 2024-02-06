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

#ifdef __linux__

#include <unistd.h>
#include <fcntl.h>

#elif _WIN32

#include <ws2tcpip.h>

#endif

// NanoNet
#include "AddrPort.h"

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

    // Linux   : int (fd)
    // Windows : SOCKET
    sock_t socket_;

#ifdef _WIN32
    bool sock_open_;
#endif

    // local address
    sockaddr_in local_;

protected:

    // ctor
    SocketBase(int type);

public:
    // dtor
    virtual ~SocketBase() = default;

    // socket
    virtual void close();
    virtual bool is_open() const;
    virtual sock_t get_sock() const;

    // bind local
    void bind(const Addr& addr);
    void bind(const Addr& addr, const Port& port);

    // get local
    AddrPort get_local() const;

    // non-blocking
    bool set_blocking(bool blocking);

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

#endif // NANONET_SOCKET_BASE_H
