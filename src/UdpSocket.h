// File:     src/UdpSocket.h
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
#ifndef NANONET_UDP_SOCKET_H
#define NANONET_UDP_SOCKET_H

// NanoNet
#include "TransSocket.h"

namespace nano {

class UdpSocket : public TransSocket {

    // is connected
    bool is_connected_;

public:

    // ctor & dtor
    UdpSocket();
    virtual ~UdpSocket() = default;

    // bind local
    virtual void bind(const Addr& addr, const Port& port) override;
    void bind(const Addr& addr);

    // send to the specified remote
    int send_to(const char* msg, size_t length, const AddrPort& remote);
    int send_to(const std::string& msg, const AddrPort& remote);

    // receive from the specified remote
    int receive_from(char* buf, size_t buf_size, AddrPort& addrport);
    int receive_from(char* buf, size_t buf_size);

    // connect to remote
    virtual void connect(const Addr& addr, const Port& port) override;
    void connect(const AddrPort& addrport);

    // send & receive
    virtual int send(const char* msg, size_t length) const override;
    virtual int send(const std::string& msg) const override;
    virtual int receive(char* buf, size_t buf_size) const override;

    // set receive timeout
    bool recv_timeout(long ms) const;

    // getter
    AddrPort get_remote() const;

}; // class UdpSocket

} // namespace nano

#endif // NANONET_UDP_SOCKET_H
