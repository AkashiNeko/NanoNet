// File:     src/ServerSocket.h
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
#ifndef NANONET_SERVER_SOCKET_H
#define NANONET_SERVER_SOCKET_H

// NanoNet
#include "Socket.h"

namespace nano {

class ServerSocket : public SocketBase {
public:

    // ctor & dtor
    ServerSocket(bool create = true);
    ServerSocket(const Addr& addr, const Port& port);
    ServerSocket(const AddrPort& addrport);
    virtual ~ServerSocket() = default;

    // move
    ServerSocket(ServerSocket&&) = default;
    ServerSocket& operator=(ServerSocket&&) = default;

    // uncopyable
    ServerSocket(const ServerSocket&) = delete;
    ServerSocket& operator=(const ServerSocket&) = delete;

    // listen
    void listen(int backlog = 20);

    // accept from client
    Socket accept();

    // set address reuse
    bool reuse_addr(bool reuseAddr) noexcept;

protected:
    virtual const char* except_name() const noexcept override;

}; // class ServerSocket

} // namespace nano

#endif // NANONET_SERVER_SOCKET_H
