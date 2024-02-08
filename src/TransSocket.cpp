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

TransSocket::TransSocket() : SocketBase(), remote_({})  {}

TransSocket::TransSocket(int type) : SocketBase(type), remote_({}) {
    remote_.sin_family = AF_INET;
}

// send to remote
int TransSocket::send_(const char* msg, size_t length, int type) const {
    // send
    int len = ::send(socket_, msg, length, 0);
    assert_throw_nanoexcept(len >= 0,
        (type == SOCK_STREAM ? "[TCP]" : "[UDP]"),
        " send(): ", LAST_ERROR);

    // returns the number of bytes sent
    return len;
}

int TransSocket::receive_(char* buf, size_t buf_size, int type) const {
    int len = static_cast<int>(::recv(socket_, buf, buf_size, 0));
    if (len == -1) {
#ifdef NANO_LINUX
        int err_code = errno, would_block = EWOULDBLOCK;
#elif NANO_WINDOWS
        int err_code = WSAGetLastError(),would_block = WSAEWOULDBLOCK;
#endif
        assert_throw_nanoexcept(err_code == would_block,
            (type == SOCK_STREAM ? "[TCP]" : "[UDP]"),
            " receive(): ", LAST_ERROR);
        return -1;
    }
    // truncate buffer
    if (len < buf_size) buf[len] = 0;
    return len;
}

} // namespace nano
