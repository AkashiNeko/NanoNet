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

class AddrPort {

    // IP Address (network byte order)
    addr_t addr;
    // Port number (network byte order)
    port_t port;

public:
    // default constructor
    AddrPort() : addr(0), port(0) {}

    // construct from address and port (network byte order)
    AddrPort(addr_t addr, port_t port) :addr(addr), port(port) {}

    AddrPort(std::string ip, port_t port) :addr(0), port(htons(port)) { this->setAddr(ip); }

    // get address (network byte order)
    inline addr_t getNetAddr() const { return addr; }

    // get address (byte order address)
    inline addr_t getAddr() const { return ntohl(addr); }

    // get port (network byte order)
    inline port_t getNetPort() const { return port; }

    // get port (host byte order)
    inline port_t getPort() const { return ntohs(port); }

    // set address (from network byte order)
    inline void setNetAddr(addr_t addr) { this->addr = addr; }

    // set address (from host byte order)
    inline void setAddr(addr_t addr) { this->addr = htonl(addr); }

    // set address by string (from host byte order)
    inline void setAddr(std::string strAddr) {
        addr = (addr_t)inet_addr(strAddr.c_str());
        assert(addr != INADDR_NONE);
    }

    // set port (from network byte order)
    inline void setNetPort(port_t port) { this->port = port; }

    // set port (from host byte order)
    inline void setPort(port_t port) { this->port = htons(port); }

    // get string address "xxx.xxx.xxx.xxx"
    std::string getStrAddr() const {
        struct in_addr inAddr;
        inAddr.s_addr = addr;
        char strAddr[INET_ADDRSTRLEN];
        const char* result = inet_ntop(AF_INET, &(inAddr.s_addr), strAddr, sizeof(strAddr));
        return result == nullptr ? "" : strAddr;
    }

    // to string "xxx.xxx.xxx.xxx:port"
    std::string toString() const {
        return this->getStrAddr() + ":" + std::to_string(ntohs(port));
    }

}; // class IPPort

} // namespace nanonet

#endif // __IPPORT_HPP__