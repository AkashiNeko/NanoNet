// addr.h

#pragma once
#ifndef NANONET_ADDR_H_
#define NANONET_ADDR_H_

// C
#include <cstring>

// C++
#include <string>

#ifdef __linux__

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#elif _WIN32

#include <WinSock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

#endif

// nanonet
#include "except.h"

namespace nano {

#ifdef __linux__
    using addr_t = in_addr_t;
#elif _WIN32
    using addr_t = ULONG;
#endif

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

}  // namespace nano

#endif  // NANONET_ADDR_H_
