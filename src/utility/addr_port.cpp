// src/utility/addr_port.cpp

#include "utility/addr_port.hpp"

namespace nanonet {

// construct from address and port (host byte order)
AddrPort::AddrPort(const Addr& addr, const Port& port) :addr(addr), port(port) {}

// to string "xxx.xxx.xxx.xxx:port"
std::string AddrPort::toString() const {
    return addr.toString() + ":" + port.toString();
}

} // namespace nanonet