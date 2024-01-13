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
    Addr& operator=(in_addr_t val);
    Addr& operator=(const char* val);
    Addr& operator=(const std::string& val);
    bool operator==(in_addr_t val);
    bool operator!=(in_addr_t val);
    bool operator==(const char* val);
    bool operator!=(const char* val);
    bool operator==(const std::string& val);
    bool operator!=(const std::string& val);

    // to network byte order
    in_addr_t net_order() const;

    // setter & getter
    void set(in_addr_t val);
    in_addr_t get() const;

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