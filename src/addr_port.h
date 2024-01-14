// addr_port.h

#pragma once
#ifndef __NANONET__ADDR_PORT_H__
#define __NANONET__ADDR_PORT_H__

// nanonet
#include "addr.h"
#include "port.h"

namespace nano {

class AddrPort {

    Addr addr;
    Port port;

public:

    // ctor & dtor
    AddrPort() = default;
    AddrPort(const Addr& addr, const Port& port);
    virtual ~AddrPort() = default;

    // getter & setter
    Addr get_addr() const;
    Port get_port() const;
    void set_addr(const Addr& addr);
    void set_port(const Port& port);

    // to string
    std::string to_string() const;
    static std::string to_string(const Addr& addr, const Port& port, char separator = ':');

}; // class addrPort

} // namespace nano

#endif // __NANONET__ADDR_PORT_H__