// File:     src/ServerSocket.cpp
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

#include "ServerSocket.h"

namespace nano {

// constructor
ServerSocket::ServerSocket(bool create)
    : SocketBase(create ? SOCK_STREAM : NULL_SOCKET) {}

ServerSocket::ServerSocket(const Addr& addr, const Port& port)
        : SocketBase(SOCK_STREAM) {
    this->reuse_addr(true);
    SocketBase::bind(addr, port);
}

ServerSocket::ServerSocket(const AddrPort& addrport)
    : ServerSocket(addrport.addr(), addrport.port()) {}

// listen
void ServerSocket::listen(int backlog) {
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET, 
        except_name(), "listen(): Socket is closed");
    assert_throw_nanoexcept(enable_listening(socket_, backlog),
        except_name(), "listen(): ", LAST_ERROR);
}

// accept a new connection
Socket ServerSocket::accept() {
    Socket ret(false);
    ret.socket_ = accept_from(socket_,
        &ret.remote_addr_, &ret.remote_port_);
    get_local_address(ret.socket_, &ret.local_addr_, &ret.local_port_);
    assert_throw_nanoexcept(ret.socket_ >= 0,
        except_name(), "accept(): ", LAST_ERROR);
    return std::move(ret);
}

// set address reuse
bool ServerSocket::reuse_addr(bool enable) noexcept {
    return this->set_option(SOL_SOCKET, SO_REUSEADDR, (int)enable);
}

const char* ServerSocket::except_name() const noexcept {
    return "[TCP] ";
}

} // namespace nano