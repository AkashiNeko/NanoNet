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
    WSAInit() {
        WSADATA wsa;
        int result = WSAStartup(MAKEWORD(2, 2), &wsa);
        assert_throw_nanoexcept(result == 0, "[WSA] WSAStartup() failed");
    }
    ~WSAInit() {
        WSACleanup();
    }
    static WSAInit _;
}

} // anonymous namespace

#endif

addr_t addr_ntoh(addr_t addr) noexcept {
    return static_cast<addr_t>(::ntohl(
        static_cast<uint32_t>(addr)));
}

addr_t addr_hton(addr_t addr) noexcept {
    return static_cast<addr_t>(::htonl(
        static_cast<uint32_t>(addr)));
}

port_t port_ntoh(port_t addr) noexcept {
    return static_cast<port_t>(::ntohs(
        static_cast<uint16_t>(addr)));
}

port_t port_hton(port_t addr) noexcept {
    return static_cast<port_t>(::htons(
        static_cast<uint16_t>(addr)));
}

addr_t addr_ston(std::string_view str) {
    addr_t addr;
    switch (inet_pton(AF_INET, str.data(), &addr))
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

std::string addr_ntos(addr_t addr) {
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
    DWORD length = sizeof(buf);
    assert_throw_nanoexcept(0 == WSAAddressToStringA(
        reinterpret_cast<LPSOCKADDR>(&sa), sizeof(sa),
        NULL, buf, &length), LAST_ERROR);
#endif
    return std::string(buf);
}

bool is_valid_ipv4(std::string_view addr) noexcept {
    int count = 0, value = 0;
    char prev = '.';
    for (const char& c : addr) {
        if (c == '.') {
            if (prev == '.') return false;
            if (value > 255 || count == 3)
                return false;
            value = 0;
            ++count;
        } else if (c >= '0' && c <= '9') {
            value = value * 10 + (c & 0xF);
        } else {
            return false;
        }
        prev = c;
    }
    return (value <= 255) && (count == 3);
}

size_t dns_query(std::string_view name,
        std::vector<addr_t>& results, int protocol) {
    addrinfo info {};
    info.ai_family = AF_INET;
    info.ai_socktype = protocol;
    addrinfo* addrs = nullptr;
    int status = getaddrinfo(name.data(), nullptr, &info, &addrs);
    results.clear();
    if (status == 0) {
        for (addrinfo* p = addrs; p; p = p->ai_next) {
            if (p->ai_family == AF_INET) {
                results.push_back(reinterpret_cast<sockaddr_in*>
                    (p->ai_addr)->sin_addr.s_addr);
            }
        }
    }
    freeaddrinfo(addrs);
    assert_throw_nanoexcept(status == 0, gai_strerror(status));
    return results.size();
}

addr_t dns_query_single(std::string_view name, int protocol) {
    addrinfo info {};
    info.ai_family = AF_INET;
    info.ai_socktype = protocol;
    addrinfo* addrs = nullptr;
    int status = getaddrinfo(name.data(), nullptr, &info, &addrs);
    addr_t result = static_cast<addr_t>(reinterpret_cast<sockaddr_in*>
        (addrs->ai_addr)->sin_addr.s_addr);
    freeaddrinfo(addrs);
    assert_throw_nanoexcept(status == 0, gai_strerror(status));
    return result;
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

sock_t accept_from(sock_t socket, addr_t* addr, port_t* port) noexcept {
    sockaddr_in remote {};
    socklen_t socklen = sizeof(remote);
    sock_t link_socket = ::accept(socket,
        reinterpret_cast<sockaddr*>(&remote), &socklen);
    if (addr) *addr = remote.sin_addr.s_addr;
    if (port) *port = remote.sin_port;
    return link_socket;
}

bool enable_listening(sock_t socket, int backlog) noexcept {
    return 0 == ::listen(socket, backlog);
}

bool connect_to(sock_t socket, addr_t addr, port_t port) noexcept {
    sockaddr_in remote {};
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = addr;
    remote.sin_port = port;
    int ret = ::connect(socket, (const sockaddr*)&remote, sizeof(remote));
    return ret == 0;
}

int recv_msg(sock_t socket, char* buf, size_t buf_size, int flags) {
    int len = static_cast<int>(::recv(socket, buf, buf_size, flags));
    if (len >= 0) {
        // truncate buffer
        if (len < buf_size) buf[len] = 0;
        return len;
    } else {
        return -ERR_CODE;
    }
}

int recv_msg_from(sock_t socket, char* buf, size_t buf_size,
        addr_t* addr, port_t* port, int flags) {
    sockaddr_in remote;
    socklen_t socklen = sizeof(remote);
    int len = static_cast<int>(::recvfrom(socket, buf, buf_size,
        flags, reinterpret_cast<sockaddr*>(&remote), &socklen));
    if (len >= 0) {
        // truncate buffer
        if (len < buf_size) buf[len] = 0;
        if (addr) *addr = remote.sin_addr.s_addr;
        if (port) *port = remote.sin_port;
        return len;
    } else {
        return -ERR_CODE;
    }
}

int send_msg(sock_t socket, const char* msg, size_t length, int flags) {
    int len = static_cast<int>(::send(socket, msg, length, flags));
    assert_throw_nanoexcept(len >= 0, LAST_ERROR);
    return len;
}

int send_msg_to(sock_t socket, const char* msg, size_t length,
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

bool close_socket(sock_t socket) noexcept {
#ifdef NANO_LINUX
    return 0 == ::close(socket);
#elif NANO_WINDOWS
    return 0 == ::closesocket(socket);
#endif
}

void make_sockaddr4(sockaddr_in* sockaddr, addr_t addr, port_t port) noexcept {
    sockaddr->sin_family = AF_INET;
    sockaddr->sin_addr.s_addr = addr;
    sockaddr->sin_port = port;
}

void get_local_address(sock_t socket, addr_t* addr, port_t* port) noexcept {
    sockaddr_in local {};
    local.sin_family = AF_INET;
    socklen_t addr_len = sizeof(local);
    ::getsockname(socket, reinterpret_cast<sockaddr*>(&local), &addr_len);
    if (addr) *addr = local.sin_addr.s_addr;
    if (port) *port = local.sin_port;
}

// Set non-blocking
bool set_blocking(sock_t socket, bool blocking) noexcept {
#ifdef NANO_LINUX
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) return false;
    if (blocking) flags &= ~O_NONBLOCK;
    else flags |= O_NONBLOCK;
    return 0 == fcntl(socket, F_SETFL, flags);
#elif NANO_WINDOWS
    u_long mode = blocking ? 0 : 1;
    return 0 == ioctlsocket(socket, FIONBIO, &mode);
#endif
}

} // namespace nano
