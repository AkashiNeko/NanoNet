// File:     src/Socket.h
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
#ifndef NANONET_SOCKET_H
#define NANONET_SOCKET_H

// NanoNet
#include "SocketBase.h"

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
    int receive(char* buf, size_t buf_size) const;

    // set receive timeout
    bool recv_timeout(long ms) const;

    // get remote
    AddrPort get_remote() const;

}; // class Socket

} // namespace nano

#endif // NANONET_SOCKET_H
