// File:     nanonet.h
// Author:   AkashiNeko
// Project:  NanoNet - Version 2.3
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
#ifndef __NANONET__
#define __NANONET__ 2.3

#if __cplusplus < 201703L
#error "Nanonet requires at least C++11"
#endif

// C++
#include <string>
#include <vector>

// platform
#ifdef __linux__ // Linux

#define NANO_LINUX 1
#define INVALID_SOCKET (-1)

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#elif _WIN32 // Windows

#define NANO_WINDOWS 1

#include <WinSock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>

#ifdef _MSC_VER // MSVC
#pragma comment(lib, "ws2_32.lib")
#endif

#else // Other platforms
#error "Unsupported platform. Only Windows and Linux are supported."
#endif

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
}; // protocol type

class NanoExcept : public std::exception {
    std::string except_msg_;
public:
    explicit NanoExcept(const std::string& msg) : except_msg_(msg) {}
    explicit NanoExcept(std::string&& msg) : except_msg_(std::move(msg)) {}
    virtual ~NanoExcept() override = default;
    virtual const char* what() const noexcept override {
        return except_msg_.c_str();
    }
};

// Convert network byte order and host byte order
addr_t addr_ntoh(addr_t addr) noexcept;
addr_t addr_hton(addr_t addr) noexcept;
port_t port_ntoh(port_t addr) noexcept;
port_t port_hton(port_t addr) noexcept;

// Converts an ip address to a numeric value and a dotted-decimal string
addr_t addr_ston(std::string_view str);
std::string addr_ntos(addr_t addr);

bool is_valid_ipv4(std::string_view addr) noexcept;

// Query the ip address corresponding to the domain name
size_t dns_query(std::string_view name, std::vector<addr_t>& results,
    int protocol = SOCK_DGRAM);

addr_t dns_query_single(std::string_view name,
    int protocol = SOCK_DGRAM);

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

// Create an ipv4 sockaddr in object
void make_sockaddr4(sockaddr_in* sockaddr,
    addr_t addr = 0, port_t port = 0) noexcept;

// Gets the address and port bound on the file descriptor
void get_local_address(sock_t socket, addr_t* addr, port_t* port) noexcept;

// Set non-blocking
bool set_blocking(sock_t socket, bool blocking) noexcept;

// Classes

class Addr {

    // net byte order addr (ipv4)
    addr_t val_;

public:

    // ctor & dtor
    Addr(addr_t val = 0) noexcept;
    Addr(std::string_view addr);

    Addr(const Addr&) = default;
    Addr(Addr&&) = default;
    virtual ~Addr() = default;

    // assignment
    Addr& operator=(const Addr&) = default;
    Addr& operator=(Addr&&) = default;

    Addr& operator=(addr_t other) noexcept;
    Addr& operator=(std::string_view addr);

    bool operator==(addr_t other) const noexcept;
    bool operator==(std::string_view other) const;

    bool operator!=(addr_t other) const noexcept;
    bool operator!=(std::string_view other) const;

    // setter & getter
    addr_t get(bool net_order = true) const noexcept;
    void set(addr_t val) noexcept;

    // to string
    std::string to_string() const noexcept;

};  // class Addr

class Port {

    // net byte order port
    port_t val_;

public:

    // ctor & dtor
    Port(port_t val = 0) noexcept;
    Port(std::string_view port);

    Port(const Port&) = default;
    Port(Port&&) = default;
    virtual ~Port() = default;

    // assignment
    Port& operator=(const Port&) = default;
    Port& operator=(Port&&) = default;

    Port& operator=(port_t other) noexcept;
    Port& operator=(std::string_view other);

    bool operator==(port_t other) const noexcept;
    bool operator==(std::string_view other) const;

    bool operator!=(port_t other) const noexcept;
    bool operator!=(std::string_view other) const;

    // getter & setter
    port_t get(bool net_order = true) const noexcept;
    void set(port_t val) noexcept;

    // to string
    std::string to_string() const noexcept;

}; // class Port

class AddrPort {

    Addr addr_;
    Port port_;

public:

    // ctor & dtor
    AddrPort() = default;
    AddrPort(const Addr& addr, const Port& port) noexcept;
    AddrPort(std::string_view addrport, char separator = ':');

    AddrPort(const AddrPort&) = default;
    AddrPort(AddrPort&&) = default;
    virtual ~AddrPort() = default;

    // assignment
    AddrPort& operator=(const AddrPort&) = default;
    AddrPort& operator=(AddrPort&&) = default;

    // getter & setter
    Addr addr() const noexcept;
    void addr(const Addr& addr) noexcept;

    Port port() const noexcept;
    void port(const Port& port) noexcept;

    // to string
    std::string to_string(char separator = ':') const noexcept;

}; // class AddrPort

class SocketBase {
protected:

    sock_t socket_;

    // local address
    addr_t local_addr_;
    port_t local_port_;

protected:

    // ctor & dtor
    SocketBase(int type);
    virtual ~SocketBase() = default;

    // move
    SocketBase(SocketBase&& other) noexcept;
    SocketBase& operator=(SocketBase&& other) noexcept;

    // uncopyable
    SocketBase(const SocketBase&) = delete;
    SocketBase& operator=(const SocketBase&) = delete;

public:

    // socket
    void close() noexcept;
    bool is_open() const noexcept;
    sock_t get() const noexcept;

    // bind local
    void bind(const Addr& addr, const Port& port);
    void bind(const AddrPort& addrport);

    // get local
    AddrPort local() const noexcept;

    // blocking
    bool set_blocking(bool blocking);

    // socket option
    template <class Ty>
    inline bool set_option(int level, int optname, const Ty& optval) const {
#ifdef NANO_LINUX
        using const_arg_t = const void*;
#elif NANO_WINDOWS
        using const_arg_t = const char*;
#endif
        return 0 == ::setsockopt(socket_, level, optname,
            (const_arg_t)&optval, sizeof(optval));
    }

    template <class Ty>
    inline bool get_option(int level, int optname, Ty& optval) const {
        socklen_t socklen = sizeof(optval);
#ifdef NANO_LINUX
        using arg_t = void*;
#elif NANO_WINDOWS
        using arg_t = char*;
#endif
        return ::getsockopt(socket_, level, optname,
            (arg_t)&optval, &socklen) == 0;
    }

protected:
    virtual const char* except_name() const noexcept;

}; // class SocketBase

class TransSocket : public SocketBase {
protected:
    // remote address
    addr_t remote_addr_;
    port_t remote_port_;

    // ctor & dtor
    TransSocket(int type);
    virtual ~TransSocket() = default;

    // move
    TransSocket(TransSocket&& other) noexcept;
    TransSocket& operator=(TransSocket&& other) noexcept;

    // uncopyable
    TransSocket(const TransSocket&) = delete;
    TransSocket& operator=(const TransSocket&) = delete;

public:

    AddrPort remote() const noexcept;

    // connect to remote
    void connect(const Addr& addr, const Port& port);
    int send(const char* msg, size_t length);
    int receive(char* buf, size_t buf_size);

    bool recv_timeout(long ms) const noexcept;

}; // class TransSocket

class Socket : public TransSocket {

    // server socket
    friend class ServerSocket;

public:

    // ctor & dtor
    Socket(bool create = true);
    virtual ~Socket() = default;

    // move
    Socket(Socket&&) = default;
    Socket& operator=(Socket&&) = default;

    // uncopyable
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

protected:
    virtual const char* except_name() const noexcept override;

}; // class Socket

class UdpSocket : public TransSocket {
public:

    // ctor & dtor
    UdpSocket(bool create = true);
    UdpSocket(const Addr& addr, const Port& port);
    virtual ~UdpSocket() = default;

    // move
    UdpSocket(UdpSocket&&) = default;
    UdpSocket& operator=(UdpSocket&&) = default;

    // uncopyable
    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;

    // send to the specified remote
    int send_to(const char* msg, size_t length, const AddrPort& remote);

    // receive from the specified remote
    int receive_from(char* buf, size_t buf_size, AddrPort& addrport);
    int receive_from(char* buf, size_t buf_size);

protected:
    virtual const char* except_name() const noexcept override;

}; // class UdpSocket

class ServerSocket : public SocketBase {
public:

    // ctor & dtor
    ServerSocket(bool create = true);
    ServerSocket(const Addr& addr, const Port& port);
    ServerSocket(const AddrPort& addrport);
    virtual ~ServerSocket() = default;

    // move
    ServerSocket(ServerSocket&&) = default;
    ServerSocket& operator=(ServerSocket&&) = default;

    // uncopyable
    ServerSocket(const ServerSocket&) = delete;
    ServerSocket& operator=(const ServerSocket&) = delete;

    // listen
    void listen(int backlog = 20);

    // accept from client
    Socket accept();

    // set address reuse
    bool reuse_addr(bool reuseAddr) noexcept;

protected:
    virtual const char* except_name() const noexcept override;

}; // class ServerSocket

} // namespace nano

#endif // __NANONET__
