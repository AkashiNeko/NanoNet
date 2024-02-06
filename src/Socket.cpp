// File:     src/Socket.cpp
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

#include "Socket.h"

namespace nano {

// null socket factory
Socket::Socket(bool, bool, bool) : TransSocket() {}

// constructor
Socket::Socket() : TransSocket(SOCK_STREAM) {}

void Socket::bind(const Addr& addr) {
    SocketBase::bind(addr, (port_t)0);
}

// connect to remote
void Socket::connect(const Addr& addr, const Port& port) {
    // set remote
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET,
        "[TCP] connect(): Socket is closed");
    remote_.sin_addr.s_addr = addr.net_order();
    remote_.sin_port = port.net_order();
    // connect to remote
    int ret = ::connect(socket_, (const sockaddr*)&remote_, sizeof(remote_));
    assert_throw_nanoexcept(ret >= 0, "[TCP] connect(): ", LAST_ERROR);
    // get local
    socklen_t addr_len = sizeof(local_);
    getsockname(socket_, (sockaddr *)&local_, &addr_len);
}

void Socket::connect(const AddrPort& addrport) {
    this->connect(addrport.addr(), addrport.port());
}

// send to remote
int Socket::send(const char* msg, size_t size) const {
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET,
        "[TCP] send(): Socket is closed");

    // send to remote
    int len = ::send(socket_, msg, size, 0);
    assert_throw_nanoexcept(len >= 0, "[TCP] send():", LAST_ERROR);

    // returns the number of bytes sent
    return len;
}

int Socket::send(const std::string& msg) const {
    return this->send(msg.c_str(), msg.size());
}

// receive from remote
int Socket::receive(char* buf, size_t buf_size) const {
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET,
        "[TCP] receive(): Socket is closed");
    int len = static_cast<int>(::recv(socket_, buf, buf_size, 0));
    if (len == -1) {
#ifdef __linux__
        int err_code = errno, would_block = EWOULDBLOCK;
#elif _WIN32
        int err_code = WSAGetLastError(),would_block = WSAEWOULDBLOCK;
#endif
        assert_throw_nanoexcept(err_code == would_block,
            "[TCP] receive(): ", LAST_ERROR);
        return -1;
    }
    // truncate buffer
    if (len < buf_size) buf[len] = 0;
    return len;
}

bool Socket::recv_timeout(long ms) const {
    timeval tm = {ms / 1000, ms % 1000 * 1000};
    return set_option(SOL_SOCKET, SO_RCVTIMEO, tm);
}

// get remote
AddrPort Socket::get_remote() const {
    return AddrPort::to_addrport(remote_);
}

// null socket factory
Socket Socket::null_socket() {
    return Socket(false, false, false);
}

} // namespace nano
