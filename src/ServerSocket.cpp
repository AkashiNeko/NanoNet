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
ServerSocket::ServerSocket() {
    this->create_socket_(SOCK_STREAM);
}

ServerSocket::ServerSocket(const Addr& addr, const Port& port) : ServerSocket() {
    this->bind(addr, port);
}

// listen
void ServerSocket::listen(int backlog) {
    int ret = ::listen(socket_, backlog);
    assert_throw(ret >= 0, "[tcp] listen: ", strerror(errno));
}

// accept a new connection
Socket ServerSocket::accept() {
    Socket socket;
    socklen_t socklen = sizeof(socket.remote_);
    int new_fd = ::accept(socket_, (sockaddr*)&socket.remote_, &socklen);
    assert_throw(new_fd >= 0, "[tcp] accept: ", strerror(errno));
    socket.socket_ = new_fd;
    socket.local_ = this->local_;
    return socket;
}

// set addr reuse
bool ServerSocket::reuse_addr(bool enable) {
    return this->set_option(SOL_SOCKET, SO_REUSEADDR, (int)enable);
}

} // namespace nano