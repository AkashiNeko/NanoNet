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
SocketBase::SocketBase(int type) {
    local_.sin_family = AF_INET;
    socket_ = ::socket(AF_INET, type, 0);
#if _WIN32
    sock_open_ = socket_ != INVALID_SOCKET;
#endif
    assert_throw_nanoexcept(this->is_open(),
        "[Socket] Create socket faild: ", LAST_ERROR);
}

// file descriptor
void SocketBase::close() {
#ifdef __linux__
    if (socket_ != -1) {
        ::close(socket_);
        socket_ = -1;
    }
#elif _WIN32
    if (sock_open_) {
        closesocket(socket_);
        sock_open_ = false;
    }
#endif
}

bool SocketBase::is_open() const {
#ifdef __linux__
    return socket_ != -1;
#elif _WIN32
    return sock_open_;
#endif
}

sock_t SocketBase::get_sock() const {
    return socket_;
}

void SocketBase::bind(const Addr& addr, const Port& port) {
    local_.sin_addr.s_addr = addr.net_order();
    local_.sin_port = port.net_order();
    int ret = ::bind(socket_, (const sockaddr*)&local_, sizeof(local_));
    assert_throw_nanoexcept(ret >= 0, "[Socket] bind(): Bind address \'",
        AddrPort::to_string(addr, port), "\' failed: ", LAST_ERROR);
}

// get local
AddrPort SocketBase::get_local() const {
    return AddrPort::to_addrport(local_);
}

// non-blocking
bool SocketBase::set_blocking(bool blocking) {
    assert_throw_nanoexcept(this->is_open(),
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
