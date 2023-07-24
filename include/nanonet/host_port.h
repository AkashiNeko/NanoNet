#pragma once
#ifndef __IPPORT_H__
#define __IPPORT_H__

#include <stdint.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "nanonet/nanotype.h"

namespace nanonet {

class HostPort {
    host_t ip;
    port_t port;
public:
    HostPort() : ip(0), port(0) {}
    HostPort(host_t ip, port_t port) : ip(ip), port(port) {}

    inline host_t getIP() { return ip; }

    inline port_t getPort() { return port; }

    inline void setIP(host_t ip) { this->ip = ip; }

    inline void setIP(std::string ip) { this->ip = (host_t)inet_addr(ip.c_str()); }

    inline void setPort(port_t port) { this->port = port; }

    inline bool operator==(const HostPort& other) const {
        return ip == other.ip && port == other.port;
    }

    inline bool operator!=(const HostPort& other) const {
        return !(*this == other);
    }

    std::string str_host() const {
        return std::to_string(ip & 0xFF)
            + "." + std::to_string((ip >> 8) & 0xFF) 
            + "." + std::to_string((ip >> 16) & 0xFF)
            + "." + std::to_string((ip >> 24)& 0xFF);
    }

    std::string to_string() const {
        return this->str_host() + ":" + std::to_string(ntohs(port));
    }

}; // class IPPort

} // namespace nanonet

#endif // __IPPORT_H__