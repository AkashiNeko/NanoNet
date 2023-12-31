// addr.h

#pragma once
#ifndef __ADDR_H__
#define __ADDR_H__

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
#include "nanonet/utility/log.h"

namespace nanonet {

class Addr {

    // host byte order addr (ipv4)
    in_addr_t val_;

    static in_addr_t parse_(const char* addr);

public:
    // constructor
    Addr(in_addr_t val = 0U);

    Addr(const char* host);

    inline Addr(const std::string& addr) :Addr(addr.c_str()) {}

    // to string "xx.xx.xx.xx"
    std::string toString() const;

    // is valid ipv4 address
    static bool isValid(const std::string& addr);

    // DNS query
    static Addr getAddrByName(const char* domain, bool useTcp = true);

    inline static Addr getAddrByName(const std::string& domain, bool useTcp) {
        return getAddrByName(domain.c_str(), useTcp);
    }

    inline Addr& operator=(in_addr_t val) {
        this->setVal(val);
        return *this;
    }

    inline void parse(const char* val) {
        this->val_ = parse_(val);
    }

    inline void parse(const std::string& val) {
        this->val_ = parse_(val.c_str());
    }

    // host byte order -> network byte order
    inline in_addr_t hton() const {
        return ::htonl(this->val_);
    }

    inline void setVal(in_addr_t val) {
        this->val_ = val;
    }
    inline in_addr_t getVal() const {
        return this->val_;
    }

};  // class Addr

}  // namespace nanonet

#endif  // __ADDR_H__