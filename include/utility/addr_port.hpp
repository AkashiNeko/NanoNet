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

struct AddrPort {
public:

    Addr addr;
    Port port;

public:

    // construct from address and port (host byte order)
    AddrPort(const Addr& addr, const Port& port) :addr(addr), port(port) {}
    // AddrPort(const std::string& ip, const Port& port) :addr(ip), port(port) {}

    // to string "xxx.xxx.xxx.xxx:port"
    std::string toString() const { return addr.toString() + ":" + port.toString(); }

}; // class addrPort

} // namespace nanonet

#endif // __ADDR_PORT_HPP__