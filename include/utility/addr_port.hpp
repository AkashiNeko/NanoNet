// addr_port.hpp

#pragma once
#ifndef __ADDR_PORT_HPP__
#define __ADDR_PORT_HPP__

// nanonet
#include "utility/addr.hpp"
#include "utility/port.hpp"

// C
#include <cassert>
#include <cstdint>

// C++
#include <string>

namespace nanonet {

class AddrPort {

    Addr addr;
    Port port;

public:

    // construct from address and port (host byte order)
    AddrPort(const Addr& addr, const Port& port);

    // to string "xxx.xxx.xxx.xxx:port"
    std::string toString() const;

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

} // namespace nanonet

#endif // __ADDR_PORT_HPP__