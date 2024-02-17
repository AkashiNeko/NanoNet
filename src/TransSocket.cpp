// File:     src/TransSocket.cpp
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

#include "TransSocket.h"

namespace nano {

TransSocket::TransSocket(int type) : SocketBase(type),
    remote_addr_(0U), remote_port_(0U) {}

TransSocket::TransSocket(TransSocket&& other)
        : SocketBase(std::move(other)),
        remote_addr_(other.remote_addr_),
        remote_port_(other.remote_port_) {
    other.remote_addr_ = 0U;
    other.remote_port_ = 0U;
}

TransSocket& TransSocket::operator=(TransSocket&& other) {
    SocketBase::operator=(std::move(other));
    // copy
    remote_addr_ = other.remote_addr_;
    remote_port_ = other.remote_port_;
    // clear
    other.remote_addr_ = 0U;
    other.remote_port_ = 0U;
    return *this;
}

AddrPort TransSocket::remote() const {
    return AddrPort(remote_addr_, remote_port_);
}

void TransSocket::connect(const Addr& addr, const Port& port) {
    // set remote
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET,
        except_name(), "connect(): Socket is closed");
    // connect to remote
    assert_throw_nanoexcept(connect_to(socket_, addr.get(), port.get()),
        except_name(), "connect(): ", LAST_ERROR);
    // get local address and port
    get_local_address(socket_, &local_addr_, &local_port_);
}

int TransSocket::send(const char* msg, size_t length) {
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET,
        except_name(), "Socket is closed");
    try {
        return send_msg(socket_, msg, length);
    } catch (const NanoExcept& e) {
        throw_except(except_name(), e.what());
    }
    return 0; // never
}

int TransSocket::receive(char* buf, size_t buf_size) {
    try {
        return recv_msg(socket_, buf, buf_size);
    } catch (const NanoExcept& e) {
        throw_except(except_name(), e.what());
    }
    return 0; // never
}

bool TransSocket::recv_timeout(long ms) const {
    timeval tm = {ms / 1000, ms % 1000 * 1000};
    return set_option(SOL_SOCKET, SO_RCVTIMEO, tm);
}

} // namespace nano
