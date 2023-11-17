// addr.hpp

#pragma once
#ifndef __ADDR_HPP__
#define __ADDR_HPP__

// C
#include <cassert>
#include <cstdint>

// C++
#include <regex>
#include <string>

// linux
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

// nanonet
#include "utility/log.hpp"

namespace nanonet {

class Addr {

    // host byte order addr (ipv4)
    in_addr_t val;

public:
    // constructor
    Addr(in_addr_t val = 0U);

    Addr(const char* host);

    Addr(const std::string& ip);

    // to string "xx.xx.xx.xx"
    std::string toString() const;

    // is valid ipv4 address
    static bool isValid(const std::string& ip);

    // DNS query
    static Addr getAddrByName(const char* domain, bool useTCP = true);

    static Addr getAddrByName(const std::string& domain, bool useTCP = true);

    // host byte order -> network byte order
    inline in_addr_t hton() const {
        return ::htonl(this->val);
    }

    inline void setVal(in_addr_t val) {
        this->val = val;
    }
    inline in_addr_t getVal() const {
        return this->val;
    }

};  // class Addr

}  // namespace nanonet

#endif  // __ADDR_HPP__