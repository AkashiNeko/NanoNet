// addr_port.h

#pragma once
#ifndef __ADDR_PORT_H__
#define __ADDR_PORT_H__

// nanonet
#include "addr.h"
#include "port.h"

// C
#include <cassert>
#include <cstdint>

// C++
#include <string>

namespace nano {

class AddrPort {

    Addr addr;
    Port port;

public:

    AddrPort() = default;

    // construct from address and port (host byte order)
    AddrPort(const Addr& addr, const Port& port);

    // to string "xxx.xxx.xxx.xxx:port"
    std::string toString() const;

    static std::string toString(const Addr& addr, const Port& port, char separator = ':');

    // getter & setter
    inline void setAddr(const Addr& addr) {
        this->addr = addr;
    }
    inline void setPort(const Port& port) {
        this->port = port;
    }

    inline Addr getAddr() const {
        return this->addr;
    }
    inline Port getPort() const {
        return this->port;
    }

}; // class addrPort

} // namespace nano

#endif // __ADDR_PORT_H__