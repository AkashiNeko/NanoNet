// File:     src/TransSocket.h
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
#ifndef NANONET_TRANSPORT_SOCKET_H
#define NANONET_TRANSPORT_SOCKET_H

#include "SocketBase.h"

namespace nano {

class TransSocket : public SocketBase {
protected:
    // remote address
    addr_t remote_addr_;
    port_t remote_port_;

    // ctor & dtor
    TransSocket(int type);
    virtual ~TransSocket() = default;

    // move
    TransSocket(TransSocket&& other);
    TransSocket& operator=(TransSocket&& other);

    // uncopyable
    TransSocket(const TransSocket&) = delete;
    TransSocket& operator=(const TransSocket&) = delete;

public:

    AddrPort remote() const;

    // connect to remote
    void connect(const Addr& addr, const Port& port);
    int send(const char* msg, size_t length);
    int receive(char* buf, size_t buf_size);

    bool recv_timeout(long ms) const;

}; // class TransSocket

} // namespace nano

#endif // NANONET_TRANSPORT_SOCKET_H