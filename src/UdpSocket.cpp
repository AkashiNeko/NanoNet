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

namespace {

int receive_from_(sock_t sock_fd, char* buf,
        size_t buf_size, AddrPort* addrport) {
    sockaddr_in remote_addr;
    socklen_t socklen = sizeof(remote_addr);
    int len = static_cast<int>(::recvfrom(sock_fd, buf, buf_size,
        0, (sockaddr*)&remote_addr, &socklen));
    if (len == -1) {
#ifdef __linux__
        int err_code = errno, would_block = EWOULDBLOCK;
#elif _WIN32
        int err_code = WSAGetLastError(), would_block = WSAEWOULDBLOCK;
#endif
        assert_throw_nanoexcept(err_code == would_block,
            "[UDP] receive_from(): ", LAST_ERROR);
        return -1;
    }  
    // truncate buffer
    if (len < buf_size) buf[len] = 0;
    if (addrport) {
        addrport->addr(::ntohl(remote_addr.sin_addr.s_addr));
        addrport->port(::ntohs(remote_addr.sin_port));
    }
    return len;
}

} // anonymous namespace

// constructor
UdpSocket::UdpSocket() : is_connected_(false) {}

void UdpSocket::bind(const Addr& addr, const Port& port) {
    create_if_closed_(SOCK_DGRAM);
    SocketBase::bind(addr, port);
}

void UdpSocket::bind(const Addr& addr) {
    create_if_closed_(SOCK_DGRAM);
    SocketBase::bind(addr, (port_t)0);
}

// send to the specified remote
int UdpSocket::send_to(const char* msg,
        size_t length, const AddrPort& remote) {
    create_if_closed_(SOCK_DGRAM);
    sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = remote.addr().net_order();
    remote_addr.sin_port = remote.port().net_order();
    auto ret = ::sendto(socket_, msg, length, 0,
        (const sockaddr*)&remote_addr, sizeof(remote_addr));
    assert_throw_nanoexcept(ret >= 0,
        "[UDP] send_to(): ", LAST_ERROR);
    return static_cast<int>(ret);
}

int UdpSocket::send_to(const std::string& msg, const AddrPort& remote) {
    return send_to(msg.c_str(), msg.size(), remote);
}

// receive from the specified remote
int UdpSocket::receive_from(char* buf, size_t buf_size, AddrPort& addrport) {
    create_if_closed_(SOCK_DGRAM);
    return receive_from_(socket_, buf, buf_size, &addrport);
}

int UdpSocket::receive_from(char* buf, size_t buf_size) {
    create_if_closed_(SOCK_DGRAM);
    return receive_from_(socket_, buf, buf_size, nullptr);
}

// connect to remote
void UdpSocket::connect(const Addr& addr, const Port& port) {
    create_if_closed_(SOCK_DGRAM);
    assert_throw_nanoexcept(!is_connected_,
        "[UDP] connect(): Socket is connected");
    remote_.sin_addr.s_addr = addr.net_order();
    remote_.sin_port = port.net_order();

    // connect
    int ret = ::connect(socket_,(const sockaddr*)&remote_, sizeof(remote_));
    assert_throw_nanoexcept(ret >= 0, "[UDP] connect(): ", LAST_ERROR);
    is_connected_ = true;

    socklen_t addr_len = sizeof(local_);
    getsockname(socket_, (sockaddr*)&local_, &addr_len);
}

void UdpSocket::connect(const AddrPort& addrport) {
    this->connect(addrport.addr(), addrport.port());
}

// send to remote
int UdpSocket::send(const char* msg, size_t size) const {
    assert_throw_nanoexcept(this->is_open(),
        "[UDP] send(): Socket is closed");
    assert_throw_nanoexcept(is_connected_,
        "[UDP] send(): Socket is not connected");

    // send
    int len = ::send(socket_, msg, size, 0);
    assert_throw_nanoexcept(len >= 0, "[UDP] send(): ", LAST_ERROR);

    // returns the number of bytes sent
    return len;
}

int UdpSocket::send(const std::string& msg) const {
    return this->send(msg.c_str(), msg.size());
}

// receive from remote
int UdpSocket::receive(char* buf, size_t buf_size) const {
    assert_throw_nanoexcept(this->is_open(),
        "[UDP] receive(): Socket is closed");
    assert_throw_nanoexcept(is_connected_,
        "[UDP] receive(): Socket is not connected");

    int len = static_cast<int>(::recv(socket_, buf, buf_size, 0));
    if (len == -1) {
        // error
    #ifdef __linux__
        int err_code = errno, would_block = EWOULDBLOCK;
    #elif _WIN32
        int err_code = WSAGetLastError(), would_block = WSAEWOULDBLOCK;
    #endif
        assert_throw_nanoexcept(err_code == would_block,
            "[UDP] receive(): ", LAST_ERROR);
        return -1;
    }
    // truncate buffer
    if (len < buf_size) buf[len] = 0;
    return len;
}

// set receive timeout
bool UdpSocket::recv_timeout(long ms) const {
    timeval tm = {ms / 1000, ms % 1000 * 1000};
    return set_option(SOL_SOCKET, SO_RCVTIMEO, tm);
}

// getter
AddrPort UdpSocket::get_remote() const {
    assert_throw_nanoexcept(is_connected_,
        "[UDP] get_remote(): Socket is not connected");
    return AddrPort::to_addrport(remote_);
}

} // namespace nano
