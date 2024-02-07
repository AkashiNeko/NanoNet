// File:     src/SocketBase.cpp
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

#include "SocketBase.h"

namespace nano {

// init WSA
#ifdef _WIN32

namespace {

class WSAInit {
public:
    WSAInit() {
        WSADATA wsa;
        int result = WSAStartup(MAKEWORD(2, 2), &wsa);
        assert_throw_nanoexcept(result == 0, "[WSA] WSAStartup() failed");
    }
    ~WSAInit() {
        WSACleanup();
    }
}__wsa_initializer_; // RAII

} // anonymous namespace

#endif

// constructor
SocketBase::SocketBase() : socket_(INVALID_SOCKET), local_({}) {}

SocketBase::SocketBase(int type) : SocketBase() {
    local_.sin_family = AF_INET;
    socket_ = ::socket(AF_INET, type, 0);
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET,
    "[Socket] Create socket faild: ", LAST_ERROR);
}

// close socket
void SocketBase::close() {
    if (socket_ != INVALID_SOCKET) {
#ifdef __linux__
        ::close(socket_);
#elif _WIN32
        closesocket(socket_);
#endif
        socket_ = INVALID_SOCKET;
    }
}

bool SocketBase::is_open() const {
    return socket_ != INVALID_SOCKET;
}

sock_t SocketBase::get_sock() const {
    return socket_;
}

void SocketBase::bind(const Addr& addr, const Port& port) {
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET,
        "[Socket] bind(): Socket is closed");
    local_.sin_addr.s_addr = addr.net_order();
    local_.sin_port = port.net_order();
    int ret = ::bind(socket_,
        reinterpret_cast<const sockaddr*>(&local_), sizeof(local_));
    assert_throw_nanoexcept(ret >= 0, "[Socket] bind(): Bind address \'",
        AddrPort::to_string(addr, port), "\' failed: ", LAST_ERROR);
}

// get local
AddrPort SocketBase::get_local() const {
    return AddrPort::to_addrport(local_);
}

// blocking
bool SocketBase::set_blocking(bool blocking) {
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET,
        "[Socket] set_blocking(): Socket is closed");
#ifdef __linux__
    int flags = fcntl(socket_, F_GETFL, 0);
    if (flags == -1) return false;
    if (blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;
    return 0 == fcntl(socket_, F_SETFL, flags);
#elif _WIN32
    u_long mode = blocking ? 0 : 1;
    return 0 == ioctlsocket(socket_, FIONBIO, &mode);
#endif
}

} // namespace nano
