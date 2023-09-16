// addr_port.hpp

#pragma once
#ifndef __ADDR_PORT_HPP__
#define __ADDR_PORT_HPP__

// nanonet
#include "nanonet/nano_def.hpp"

// Linux
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// C
#include <cassert>
#include <cstdint>

// C++
#include <string>

namespace nanonet {

class addr_port {

    // IP Address (network byte order)
    addr_t addr_;

    // Port number (network byte order)
    port_t port_;


public:

    // default constructor
    addr_port() : addr_(0), port_(0) {}


    // construct from address and port (network byte order)
    addr_port(addr_t addr, port_t port) 
        :addr_(addr), port_(port) {}

    addr_port(std::string ip, port_t port) 
        :addr_((addr_t)::inet_addr(ip.c_str())), port_(::htons(port)) {}


    // get address (network byte order)
    inline addr_t net_addr() const { return addr_; }


    // get address (byte order address)
    inline addr_t addr() const { return ::ntohl(addr_); }


    // get port (network byte order)
    inline port_t net_port() const { return port_; }


    // get port (host byte order)
    inline port_t port() const { return ::ntohs(port_); }


    // set address (from network byte order)
    inline void net_addr(addr_t addr) { this->addr_ = addr; }


    // set address (from host byte order)
    inline void set_addr(addr_t addr) { this->addr_ = ::htonl(addr); }


    // set address by string (from host byte order)
    inline void set_addr(std::string strAddr) {
        addr_ = (addr_t)::inet_addr(strAddr.c_str());
        assert(addr_ != INADDR_NONE);
    }


    // set port (from network byte order)
    inline void set_net_port(port_t port) { this->port_ = port; }


    // set port (from host byte order)
    inline void set_port(port_t port) { this->port_ = ::htons(port); }


    // string address "xxx.xxx.xxx.xxx"
    std::string addr_str() const {
        struct in_addr inAddr;
        inAddr.s_addr = addr_;
        char strAddr[INET_ADDRSTRLEN];
        const char* result = ::inet_ntop(AF_INET, &(inAddr.s_addr), strAddr, sizeof(strAddr));
        return result == nullptr ? std::string() : strAddr;
    }


    // to string "xxx.xxx.xxx.xxx:port"
    std::string to_string() const {
        return this->addr_str() + ":" + std::to_string(ntohs(port_));
    }


}; // class addr_port

} // namespace nanonet

#endif // __ADDR_PORT_HPP__