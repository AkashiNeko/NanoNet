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
#include <utility/log.hpp>

namespace nanonet {

struct Addr {
public:
    // host byte order addr (ipv4)
    in_addr_t val;

public:
    // constructor
    Addr(in_addr_t val = 0U)
        : val(val) {}

    Addr(const char* host) {
        if (isValid(host)) {
            this->val = ::ntohl(inet_addr(host));
        } else {
            this->val = getAddrByName(host).val;
            Log::debug << "[addr] getaddrinfo " << host << " -> " << this->toString() << std::endl;
        }
    }

    Addr(const std::string& ip)
        : Addr(ip.c_str()) {}

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

    // DNS query
    static Addr getAddrByName(const char* domain, bool useTCP = true) {
        struct addrinfo hints = {}, *result, *p;
        int status;
        hints.ai_family = AF_INET;
        hints.ai_socktype = useTCP ? SOCK_STREAM : SOCK_DGRAM;
        status = getaddrinfo(domain, NULL, &hints, &result);
        if (status != 0) {
            Log::error << "[addr] getaddrinfo: " << gai_strerror(status) << std::endl;
            exit(-1);
        }
        in_addr_t addr = INADDR_ANY;
        for (p = result; p != NULL; p = p->ai_next) {
            if (p->ai_family == AF_INET) {
                struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
                addr = ipv4->sin_addr.s_addr;
                break;
            }
        }
        freeaddrinfo(result);
        return Addr(::htonl(addr));
    }

    static Addr getAddrByName(const std::string& domain, bool useTCP = true) {
        return Addr::getAddrByName(domain.c_str(), useTCP);
    }

};  // struct Addr

}  // namespace nanonet

#endif  // __ADDR_HPP__