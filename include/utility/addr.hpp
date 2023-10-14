// addr.hpp

#pragma once
#ifndef __ADDR_HPP__
#define __ADDR_HPP__

// C
#include <cstdint>
#include <cassert>

// C++
#include <string>
#include <regex>

// linux
#include <netinet/in.h>
#include <arpa/inet.h>

namespace nanonet {

struct Addr {
public:

    // ipv4 address
    in_addr_t val;

public:

    // constructor
    Addr(in_addr_t val = 0U) :val(val) {}

    Addr(const char* ip) :Addr(::ntohl(inet_addr(ip))) {}

    Addr(const std::string& ip) {
        if (isValid(ip)) {
            this->val = ::ntohl(inet_addr(ip.c_str()));
        } else {
            exit(-1);
        }
    }

    // to string "xx.xx.xx.xx"
    std::string toString() const {
        struct in_addr inAddr;
        inAddr.s_addr = ::htonl(this->val);
        char strAddr[INET_ADDRSTRLEN];
        const char* result = ::inet_ntop(AF_INET, &(inAddr.s_addr), strAddr, sizeof(strAddr));
        return result == nullptr ? std::string() : strAddr;
    }

    // host byte order -> network byte order
    in_addr_t hton() const { return ::htonl(this->val); }

    // is valid ipv4 address
    static bool isValid(const std::string& ip) {
        std::regex pattern("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
        return std::regex_match(ip, pattern);
    }

}; // struct Addr

} // namespace nanonet

#endif // __ADDR_HPP__