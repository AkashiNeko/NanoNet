// File:     src/net.cpp
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

#include "net.h"

namespace nano {

// init WSA
#ifdef NANO_WINDOWS

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

addr_t net_to_host(addr_t addr) noexcept {
    return static_cast<addr_t>(::ntohl(
        static_cast<uint32_t>(addr)));
}

addr_t host_to_net(addr_t addr) noexcept {
    return static_cast<addr_t>(::htonl(
        static_cast<uint32_t>(addr)));
}

port_t net_to_host(port_t addr) noexcept {
    return static_cast<port_t>(::ntohs(
        static_cast<uint16_t>(addr)));
}

port_t host_to_net(port_t addr) noexcept {
    return static_cast<port_t>(::htons(
        static_cast<uint16_t>(addr)));
}

addr_t parse_host_addr(const char* str) {
    addr_t addr;
#ifdef NANO_LINUX
    switch (inet_pton(AF_INET, str, &addr))
#elif NANO_WINDOWS
    switch(InetPtonA(AF_INET, str, &addr))
#endif
    {
    case 1:  // success
        return addr;
    case 0:  // invalid network address in the specified address family
        throw_except("String does not contain a valid network address");
    case -1: // af does not contain a valid address family
        throw_except(LAST_ERROR);
    default: // never
        return 0;
    }
}

std::string net_to_string(addr_t addr) {
    char buf[INET_ADDRSTRLEN] = {};
#ifdef NANO_LINUX
    in_addr inaddr = {addr};
    const char* result = ::inet_ntop(AF_INET,
        &inaddr, buf, sizeof(buf));
    assert_throw_nanoexcept(result, LAST_ERROR);
#elif NANO_WINDOWS
    sockaddr_in sa {};
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = addr;
    DWORD length = sizeof(result);
    assert_throw_nanoexcept(0 == WSAAddressToStringA(
        static_cast<LPSOCKADDR>(&sa), sizeof(sa),
        NULL, result, &length), LAST_ERROR);
#endif
    return std::string(result);
}

size_t dns_query(const char* name, addr_t addrs[],
        size_t size, int type) noexcept {
    addrinfo info {};
    info.ai_family = AF_INET;
    info.ai_socktype = type;
    addrinfo* result = nullptr;
    int status = getaddrinfo(name, nullptr, &info, &result);
    addr_t addr = INADDR_ANY;
    size_t cnt = 0;
    if (status == 0) {
        for (addrinfo* p = result; p && cnt != size; p = p->ai_next) {
            if (p->ai_family == AF_INET) {
                addrs[cnt++] = reinterpret_cast<sockaddr_in*>
                    (p->ai_addr)->sin_addr.s_addr;
            }
        }
    }
    freeaddrinfo(result);
    assert_throw_nanoexcept(status == 0, gai_strerror(status));
    return cnt;
}

sock_t create_socket(int domain, int type, int protocol) noexcept {
    return ::socket(domain, type, protocol);
}

bool bind_address(sock_t socket, addr_t addr, port_t port) noexcept {
    sockaddr_in local {};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = addr;
    local.sin_port = port;
    socklen_t len = sizeof(local);
    return 0 == ::bind(socket, reinterpret_cast<const sockaddr*>(&local), len);
}

sock_t accept(sock_t socket, addr_t* addr, port_t* port, int backlog) {
    sockaddr_in remote {};
    socklen_t socklen = sizeof(remote);
    sock_t link_socket = ::accept(socket,
        reinterpret_cast<sockaddr*>(&remote), &socklen);
    return link_socket;
}

bool connect(sock_t socket, addr_t addr, port_t port) noexcept {
    sockaddr_in remote {};
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = addr;
    remote.sin_port = port;
    int ret = ::connect(socket, (const sockaddr*)&remote, sizeof(remote));
    return ret == 0;
}

int receive(sock_t socket, char* buf, size_t buf_size, int flags) {
    int len = static_cast<int>(::recv(socket, buf, buf_size, flags));
    if (len >= 0) {
        // truncate buffer
        if (len < buf_size) buf[len] = 0;
        return len;
    }
#ifdef NANO_LINUX
    if (ERR_CODE != EWOULDBLOCK)
#elif NANO_WINDOWS
    if (ERR_CODE != WSAEWOULDBLOCK)
#endif
        throw_except(LAST_ERROR);
    return -1;
}

int receive_from(sock_t socket, char* buf, size_t buf_size,
        addr_t* addr, port_t* port, int flags) {
    sockaddr_in remote;
    socklen_t socklen = sizeof(remote);
    int len = static_cast<int>(::recvfrom(socket, buf, buf_size,
        flags, reinterpret_cast<sockaddr*>(&remote), &socklen));
    if (len >= 0) {
        // truncate buffer
        if (len < buf_size) buf[len] = 0;
        if (addr) *addr = ::ntohl(remote.sin_addr.s_addr);
        if (port) *port = ::ntohs(remote.sin_port);
        return len;
    }
#ifdef NANO_LINUX
    if (ERR_CODE != EWOULDBLOCK)
#elif NANO_WINDOWS
    if (ERR_CODE != WSAEWOULDBLOCK)
#endif
        throw_except(LAST_ERROR);
    return -1;
}

int send(sock_t socket, const char* msg, size_t length, int flags) {
    int len = static_cast<int>(::send(socket, msg, length, flags));
    assert_throw_nanoexcept(len >= 0, LAST_ERROR);
    return len;
}

int send_to(sock_t socket, const char* msg, size_t length,
        addr_t addr, port_t port, int flags) {
    sockaddr_in remote;
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = addr;
    remote.sin_port = port;
    int ret = static_cast<int>(sendto(socket, msg, length, flags,
        reinterpret_cast<const sockaddr*>(&remote), sizeof(remote)));
    assert_throw_nanoexcept(ret >= 0, LAST_ERROR);
    return ret;
}

} // namespace nano
