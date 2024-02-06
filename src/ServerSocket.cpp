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

// null socket factory
ServerSocket::ServerSocket(bool, bool, bool) : SocketBase() {}

// constructor
ServerSocket::ServerSocket() : SocketBase(SOCK_STREAM) {}

ServerSocket::ServerSocket(const Addr& addr, const Port& port)
        : SocketBase(SOCK_STREAM) {
    this->reuse_addr(true);
    SocketBase::bind(addr, port);
}

ServerSocket::ServerSocket(const AddrPort& addrport)
    : ServerSocket(addrport.addr(), addrport.port()) {}

ServerSocket::ServerSocket(const Port& port)
    : ServerSocket((addr_t)0, port) {}

// bind
void ServerSocket::bind(const Port& port) {
    SocketBase::bind((addr_t)0, port);
}

// listen
void ServerSocket::listen(int backlog) {
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET, 
        "[TCP] listen(): Socket is closed");
    assert_throw_nanoexcept(::listen(socket_, backlog) >= 0,
        "[TCP] listen(): ", LAST_ERROR);
}

// accept a new connection
Socket ServerSocket::accept() {
    Socket socket;
    socklen_t socklen = sizeof(socket.remote_);
    int new_fd = ::accept(socket_,
        reinterpret_cast<sockaddr*>(&socket.remote_), &socklen);
    assert_throw_nanoexcept(new_fd >= 0, "[TCP] accept(): ", LAST_ERROR);
    socket.socket_ = new_fd;
    socket.local_ = this->local_;
    return socket;
}

// set addr reuse
bool ServerSocket::reuse_addr(bool enable) {
    return this->set_option(SOL_SOCKET, SO_REUSEADDR, (int)enable);
}

ServerSocket ServerSocket::null_socket() {
    return ServerSocket(false, false, false);
}

} // namespace nano