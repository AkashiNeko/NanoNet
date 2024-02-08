// File:     src/net.h
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

#pragma once
#ifndef NANONET_NET_H
#define NANONET_NET_H

#if __cplusplus < 201103L
#error "Nanonet requires at least C++11"
#endif

#ifdef __linux__ // Linux

#define NANO_LINUX
#define INVALID_SOCKET (-1)

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#elif _WIN32 // Windows

#define NANO_WINDOWS

#include <WinSock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>

#ifdef _MSC_VER // MSVC
#pragma comment(lib, "ws2_32.lib")
#endif

#endif

// NanoNet
#include "except.h"

namespace nano {

#ifdef NANO_LINUX
    using sock_t = int;
    using addr_t = in_addr_t;
    using port_t = in_port_t;
#elif NANO_WINDOWS
    using sock_t = SOCKET;
    using addr_t = ULONG;
    using port_t = USHORT;
#endif

enum Domain {
    IPv4 = AF_INET,
    // IPv6 = AF_INET6, // Not supported yet (NanoNet 2.3)
};

enum SockType {
    TCP_SOCK = SOCK_STREAM,
    UDP_SOCK = SOCK_DGRAM,
    NONBLOCK = SOCK_NONBLOCK,
}; // protocol type

// Convert network byte order and host byte order
addr_t addr_ntoh(addr_t addr) noexcept;
addr_t addr_hton(addr_t addr) noexcept;
port_t port_ntoh(port_t addr) noexcept;
port_t port_hton(port_t addr) noexcept;

// Converts an ip address to a numeric value and a dotted-decimal string
addr_t addr_ston(const char* str);
std::string addr_ntos(addr_t addr);

// Query the ip address corresponding to the domain name
size_t dns_query(const char* name, addr_t addrs[], size_t size,
    int protocol = SockType::UDP_SOCK);

// Create a TCP/UDP socket
sock_t create_socket(int domain, int type, int protocol = 0) noexcept;

// Bind a address to a socket
bool bind_address(sock_t socket, addr_t addr, port_t port) noexcept;

// Accept a connection on a socket
sock_t accept_from(sock_t socket, addr_t* addr, port_t* port) noexcept;

// Listen for connections on a socket
bool enable_listening(sock_t socket, int backlog = 20) noexcept;

// Initiate a connection on a socket
bool connect_to(sock_t socket, addr_t addr, port_t port) noexcept;

// Receive a message from a socket
int recv_msg(sock_t socket, char* buf, size_t buf_size, int flags = 0);
int recv_msg_from(sock_t socket, char* buf, size_t buf_size,
    addr_t* addr, port_t* port, int flags = 0);

// Send a message on a socket
int send_msg(sock_t socket, const char* msg, size_t length, int flags = 0);
int send_msg_to(sock_t socket, const char* msg, size_t length,
    addr_t addr, port_t port, int flags = 0);

// Close the socket
bool close_socket(sock_t socket) noexcept;

} // namespace nano

#endif // NANONET_NET_H