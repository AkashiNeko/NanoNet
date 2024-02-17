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

// constructor
SocketBase::SocketBase(int type) : socket_(INVALID_SOCKET),
        local_addr_(0U), local_port_(0U) {
    if (type == NULL_SOCKET) return;
    socket_ = ::socket(AF_INET, type, 0);
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET,
    except_name(), "Create socket faild: ", LAST_ERROR);
}

SocketBase::SocketBase(SocketBase&& other) noexcept
        : socket_(other.socket_),
        local_addr_(other.local_addr_),
        local_port_(other.local_port_) {
    other.socket_ = INVALID_SOCKET;
    other.local_addr_ = 0U;
    other.local_port_ = 0U;
}

SocketBase& SocketBase::operator=(SocketBase&& other) noexcept {
    // close itself
    this->close();
    // copy
    socket_ = other.socket_;
    local_addr_ = other.local_addr_;
    local_port_ = other.local_port_;
    // clear other
    other.socket_ = INVALID_SOCKET;
    other.local_addr_ = 0U;
    other.local_port_ = 0U;
    return *this;
}

// close socket
void SocketBase::close() {
    if (socket_ != INVALID_SOCKET) {
#ifdef NANO_LINUX
        ::close(socket_);
#elif NANO_WINDOWS
        closesocket(socket_);
#endif
        socket_ = INVALID_SOCKET;
    }
}

bool SocketBase::is_open() const {
    return socket_ != INVALID_SOCKET;
}

sock_t SocketBase::get() const {
    return socket_;
}

void SocketBase::bind(const Addr& addr, const Port& port) {
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET,
        except_name(), "bind(): Socket is closed");
    assert_throw_nanoexcept(bind_address(socket_, addr.get(), port.get()),
        except_name(), "bind(): Bind address \'",
        AddrPort(addr, port).to_string(), "\' failed: ", LAST_ERROR);
}

void SocketBase::bind(const AddrPort& addrport) {
    this->bind(addrport.addr(), addrport.port());
}

// get local
AddrPort SocketBase::local() const {
    return AddrPort(local_addr_, local_port_);
}

// blocking
bool SocketBase::set_blocking(bool blocking) {
    assert_throw_nanoexcept(socket_ != INVALID_SOCKET,
        except_name(), "set_blocking(): Socket is closed");
#ifdef NANO_LINUX
    int flags = fcntl(socket_, F_GETFL, 0);
    if (flags == -1) return false;
    if (blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;
    return 0 == fcntl(socket_, F_SETFL, flags);
#elif NANO_WINDOWS
    u_long mode = blocking ? 0 : 1;
    return 0 == ioctlsocket(socket_, FIONBIO, &mode);
#endif
}

const char* SocketBase::except_name() const noexcept {
    return "[socket] ";
}

} // namespace nano
