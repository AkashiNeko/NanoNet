// addr.h

#pragma once
#ifndef __ADDR_H__
#define __ADDR_H__

// C
#include <cstdint>
#include <cstring>

// C++
#include <regex>
#include <string>

// linux
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

// nanonet
#include "except.h"

namespace nano {

class Addr {

    // host byte order addr (ipv4)
    in_addr_t val_;

public:

    Addr(in_addr_t val = 0);
    Addr(const char* addr);
    Addr(const std::string& addr);
    virtual ~Addr() = default;

    // assignment
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

}  // namespace nano

#endif  // __ADDR_H__