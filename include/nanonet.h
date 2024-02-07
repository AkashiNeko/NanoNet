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

#if __cplusplus < 201103L
    #error "Nanonet requires at least C++11"
#endif

// C++
#include <string>

// platform
#ifdef __linux__

#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#elif _WIN32

#include <WinSock2.h>

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

#else
    #error "Unsupported platform. Only Windows and Linux are supported."
#endif

namespace nano {

#ifdef __linux__
    using sock_t = int;
    using addr_t = in_addr_t;
    using port_t = in_port_t;
#elif _WIN32
    using sock_t = SOCKET;
    using addr_t = ULONG;
    using port_t = USHORT;
#endif

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

class Addr {

    // host byte order addr (ipv4)
    addr_t val_;

public:

    // ctor & dtor
    Addr(addr_t val = 0);
    Addr(const char* addr);
    Addr(const std::string& addr);
    virtual ~Addr() = default;

    // assign
    Addr& operator=(const Addr&) = default;
    Addr& operator=(addr_t other);
    Addr& operator=(const char* other);
    Addr& operator=(const std::string& other);

    bool operator==(addr_t other) const;
    bool operator!=(addr_t other) const;
    bool operator==(const char* other) const;
    bool operator!=(const char* other) const;
    bool operator==(const std::string& other) const;
    bool operator!=(const std::string& other) const;

    // to network byte order
    addr_t net_order() const;

    // setter & getter
    addr_t get() const;
    void set(addr_t val);

    // to string
    std::string to_string() const;

    // is valid ipv4 address
    static bool is_valid(const std::string& addr);

    // DNS query
    static Addr dns_query(const char* domain, bool use_tcp = true);
    static Addr dns_query(const std::string& domain, bool use_tcp = true);

};  // class Addr

class Port {

    // host byte order port
    port_t val_;

public:

    // ctor & dtor
    Port(port_t val = 0);
    Port(const char* port);
    Port(const std::string& port);
    virtual ~Port() = default;

    // assign
    Port& operator=(const Port&) = default;
    Port& operator=(port_t other);
    Port& operator=(const char* other);
    Port& operator=(const std::string& other);

    bool operator==(port_t other) const;
    bool operator!=(port_t other) const;
    bool operator==(const char* other) const;
    bool operator!=(const char* other) const;
    bool operator==(const std::string& other) const;
    bool operator!=(const std::string& other) const;

    // to network byte order
    port_t net_order() const;

    // getter & setter
    port_t get() const;
    void set(port_t val);

    // to string
    std::string to_string() const;

}; // class Port

class AddrPort {

    Addr addr_;
    Port port_;

public:

    // ctor & dtor
    AddrPort() = default;
    AddrPort(const Addr& addr, const Port& port);
    AddrPort(const char* addrport, char separator = ':');
    AddrPort(const std::string& addrport, char separator = ':');
    virtual ~AddrPort() = default;

    // getter & setter
    Addr addr() const;
    Port port() const;
    void addr(const Addr& addr);
    void port(const Port& port);

    // to string
    std::string to_string() const;
    static std::string to_string(const Addr& addr, const Port& port, char separator = ':');

    // sockaddr_in -> AddrPort
    static AddrPort to_addrport(sockaddr_in address);

}; // class AddrPort

class SocketBase {
protected:

    // Linux   : int
    // Windows : SOCKET
    sock_t socket_;

    // local address
    sockaddr_in local_;

protected:

    // ctor
    SocketBase();
    SocketBase(int type);

public:
    // dtor
    virtual ~SocketBase() = default;

    // socket
    void close();
    bool is_open() const;
    sock_t get_sock() const;

    // bind local
    virtual void bind(const Addr& addr, const Port& port);

    // get local
    AddrPort get_local() const;

    // blocking
    bool set_blocking(bool blocking);

    // set socket option
    template <class OptionType>
    inline bool set_option(int level, int optname, const OptionType& optval) const {
        return ::setsockopt(socket_, level, optname, (const char*)&optval, sizeof(optval)) == 0;
    }

    template <class OptionType>
    inline bool get_option(int level, int optname, OptionType& optval) const {
        return ::getsockopt(socket_, level, optname, (const char*)&optval, sizeof(optval)) == 0;
    }

}; // class SocketBase

class TransSocket : public SocketBase {
protected:
    // remote address
    sockaddr_in remote_;

    // ctor
    TransSocket();
    TransSocket(int type);

    int send_(const char* msg, size_t length, int type) const;
    int receive_(char* buf, size_t buf_size, int type) const;

public:
    // dtor
    virtual ~TransSocket() = default;

    // connect to remote
    virtual void connect(const Addr& addr, const Port& port) = 0;

}; // class TransSocket

class Socket : public TransSocket {

    // server socket
    friend class ServerSocket;

    // null socket factory
    explicit Socket(bool, bool, bool);

public:

    // ctor & dtor
    Socket();
    virtual ~Socket() = default;

    // connect to remote
    virtual void connect(const Addr& addr, const Port& port) override;
    void connect(const AddrPort& addrport);

    // send & receive
    int send(const char* msg, size_t length) const;
    int send(const std::string& msg) const;
    int receive(char* buf, size_t buf_size) const;

    // set receive timeout
    bool recv_timeout(long ms) const;

    // get remote
    AddrPort get_remote() const;

    static Socket null_socket();

}; // class Socket

class UdpSocket : public TransSocket {

    // is connected
    bool is_connected_;

    // null socket factory
    explicit UdpSocket(bool, bool, bool);

public:

    // ctor & dtor
    UdpSocket();
    UdpSocket(const Addr& addr, const Port& port);
    UdpSocket(const Port& port);

    virtual ~UdpSocket() = default;

    // send to the specified remote
    int send_to(const char* msg, size_t length, const AddrPort& remote);
    int send_to(const std::string& msg, const AddrPort& remote);

    // receive from the specified remote
    int receive_from(char* buf, size_t buf_size, AddrPort& addrport);
    int receive_from(char* buf, size_t buf_size);

    // connect to remote
    virtual void connect(const Addr& addr, const Port& port) override;
    void connect(const AddrPort& addrport);

    // send & receive
    int send(const char* msg, size_t length) const;
    int send(const std::string& msg) const;
    int receive(char* buf, size_t buf_size) const;

    // set receive timeout
    bool recv_timeout(long ms) const;

    // getter
    AddrPort get_remote() const;

    // null socket factory
    static UdpSocket null_socket();

}; // class UdpSocket

class ServerSocket : public SocketBase {

    // null socket factory
    explicit ServerSocket(bool, bool, bool);

public:

    // ctor & dtor
    ServerSocket();
    ServerSocket(const Addr& addr, const Port& port);
    ServerSocket(const AddrPort& addrport);
    ServerSocket(const Port& port);
    virtual ~ServerSocket() = default;

    // bind local
    void bind(const Addr& addr, const Port& port);
    void bind(const Port& port);

    // listen
    void listen(int backlog = 20);

    // accept from client
    Socket accept();

    // set address reuse
    bool reuse_addr(bool reuseAddr);

    static ServerSocket null_socket();

}; // class ServerSocket

} // namespace nano

#endif // __NANONET__
