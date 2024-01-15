// nanonet.h

#pragma once
#ifndef __NANONET_H__
#define __NANONET_H__

#if __cplusplus < 201103L
    #error "Nanonet requires at least C++11"
#endif

// C
#include <cstdint>

// C++
#include <regex>
#include <string>

// Linux
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

namespace nano {

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

    Addr addr;
    Port port;

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

} // namespace nano

#endif // __NANONET_H__
