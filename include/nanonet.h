// nanonet.h

#pragma once
#ifndef NANONET_H
#define NANONET_H

#if __cplusplus < 201103L
    #error "Nanonet requires at least C++11"
#endif

// C++
#include <string>

// Linux
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

namespace nano {

#ifdef __WIN32__ // Windows
    using sock_t = SOCKET;
#elif __linux__ // Linux
    using sock_t = int;
#else // other
    #error "Unsupported platform. Only Windows and Linux are supported."
#endif // platform

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

// throw exceptions
#if __cplusplus >= 201703L

template <class ExceptType = NanoExcept, class ...Args>
inline void throw_except(bool condition, const Args&... args) {
    if (condition) return;
    std::string s;
    ((s += args), ...);
    throw ExceptType(std::move(s));
}

#else // 201103L <= __cplusplus < 201703L

inline void append_string_(std::string&) {}

template <class T, class ...Args>
inline void append_string_(std::string& s, const T& arg, const Args&... args) {
    s += arg;
    append_string_(s, args...);
}

template <class ExceptType = NanoExcept, class ...Args>
inline void assert_throw(bool condition, const Args&... args) {
    if (condition) return;
    std::string s;
    append_string_(s, args...);
    throw ExceptType(std::move(s));
}

#endif // __cplusplus
class Addr {

    // host byte order addr (ipv4)
    in_addr_t val_;

public:

    // ctor & dtor
    Addr(in_addr_t val = 0);
    Addr(const char* addr);
    Addr(const std::string& addr);
    virtual ~Addr() = default;

    // assign
    Addr& operator=(const Addr&) = default;
    Addr& operator=(in_addr_t other);
    Addr& operator=(const char* other);
    Addr& operator=(const std::string& other);

    bool operator==(in_addr_t other) const;
    bool operator!=(in_addr_t other) const;
    bool operator==(const char* other) const;
    bool operator!=(const char* other) const;
    bool operator==(const std::string& other) const;
    bool operator!=(const std::string& other) const;

    // to network byte order
    in_addr_t net_order() const;

    // setter & getter
    in_addr_t get() const;
    void set(in_addr_t val);

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
    in_port_t val_;

public:

    // ctor & dtor
    Port(in_port_t val = 0);
    Port(const char* port);
    Port(const std::string& port);
    virtual ~Port() = default;

    // assign
    Port& operator=(in_port_t other);
    Port& operator=(const char* other);
    Port& operator=(const std::string& other);

    bool operator==(in_port_t other) const;
    bool operator!=(in_port_t other) const;
    bool operator==(const char* other) const;
    bool operator!=(const char* other) const;
    bool operator==(const std::string& other) const;
    bool operator!=(const std::string& other) const;

    // to network byte order
    in_port_t net_order() const;

    // getter & setter
    in_addr_t get() const;
    void set(in_addr_t val);

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
    virtual ~AddrPort() = default;

    // getter & setter
    Addr get_addr() const;
    Port get_port() const;
    void set_addr(const Addr& addr);
    void set_port(const Port& port);

    // to string
    std::string to_string() const;
    static std::string to_string(const Addr& addr, const Port& port, char separator = ':');

}; // class AddrPort

class SocketBase {
protected:

    // fd of socket
    int sock_fd_;

    // local address
    sockaddr_in local_;

public:

    // ctor & dtor
    SocketBase(int fd = -1);
    virtual ~SocketBase() = default;

    // file descriptor
    virtual void close();
    virtual bool is_open() const;
    virtual int get_fd() const;

    // bind local
    void bind(const Addr& addr, const Port& port);

    // get local
    AddrPort get_local() const;

    // set socket option
    template <class OptionType>
    inline bool set_option(int level, int optname, const OptionType& optval) const {
        return ::setsockopt(sock_fd_, level, optname, &optval, sizeof(optval)) == 0;
    }

    template <class OptionType>
    inline bool get_option(int level, int optname, OptionType& optval) const {
        return ::getsockopt(sock_fd_, level, optname, &optval, sizeof(optval)) == 0;
    }

}; // class SocketBase

class Socket : public SocketBase {

    // remote address
    sockaddr_in remote_;

    // server socket
    friend class ServerSocket;

public:

    // ctor & dtor
    Socket();
    virtual ~Socket() = default;

    // connect to remote
    void connect(const Addr& addr, const Port& port);

    // send to remote
    int send(const char* msg, size_t length) const;
    int send(const std::string msg) const;

    // receive from remote
    int receive(char *buf, size_t buf_size) const;

    // set receive timeout
    bool recv_timeout(long ms) const;

    // get remote
    AddrPort get_remote() const;

}; // class Socket

class ServerSocket : public SocketBase {
public:

    // ctor & dtor
    ServerSocket();
    ServerSocket(const Addr& addr, const Port& port);
    virtual ~ServerSocket() = default;

    // listen
    void listen(int backlog = 20);

    // accept from client
    Socket accept();

    // set address reuse
    void reuse_addr(bool reuseAddr);

}; // class ServerSocket

class UdpSocket : public SocketBase {

    // remote address
    sockaddr_in remote_;

    // is connected
    bool is_connected_;

public:

    // ctor & dtor
    UdpSocket();
    virtual ~UdpSocket() = default;

    // send to the specified remote
    int send_to(const char* msg, size_t length, const AddrPort& remote) const;
    int send_to(const std::string& msg, const AddrPort& remote) const;

    // receive from the specified remote
    int receive_from(char* buf, size_t buf_size, AddrPort& addrport) const;
    int receive_from(char* buf, size_t buf_size) const;

    // connect to remote
    void connect(const Addr& addr, const Port& port);

    // send & receive
    int send(const char* msg, size_t length) const;
    int send(const std::string& msg) const;
    int receive(char* buf, size_t buf_size) const;

    // set receive timeout
    bool recv_timeout(long ms) const;

    // getter
    AddrPort get_remote() const;

}; // class UdpSocket

} // namespace nano

#endif // NANONET_H
