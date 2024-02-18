// File:     src/UdpSocket
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

#include "UdpSocket.h"

namespace nano {

// constructor
UdpSocket::UdpSocket(bool create)
    : TransSocket(create ? SOCK_DGRAM : NULL_SOCKET) {}

UdpSocket::UdpSocket(const Addr& addr, const Port& port)
        : TransSocket(SOCK_DGRAM) {
    SocketBase::bind(addr, port);
}

// send to the specified remote
int UdpSocket::send_to(const char* msg,
        size_t length, const AddrPort& remote) {
    try {
        return send_msg_to(socket_, msg, length,
            remote.addr().get(), remote.port().get());
    } catch (const NanoExcept& e) {
        throw_except(except_name(), e.what());
    }
    return 0; // never
}

// receive from the specified remote
int UdpSocket::receive_from(char* buf, size_t buf_size, AddrPort& addrport) {
    addr_t addr = 0;
    port_t port = 0;
    int ret = recv_msg_from(socket_, buf, buf_size, &addr, &port);
    addrport.addr(addr_ntoh(addr));
    addrport.port(port_ntoh(port));
    return ret;
}

int UdpSocket::receive_from(char* buf, size_t buf_size) {
    return recv_msg_from(socket_, buf, buf_size, nullptr, nullptr);
}

const char* UdpSocket::except_name() const noexcept {
    return "[UDP] ";
}

} // namespace nano
