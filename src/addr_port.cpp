// addr_port.cpp

#include "addr_port.h"

namespace nano {

// construct from address and port (host byte order)
AddrPort::AddrPort(const Addr& addr, const Port& port) :addr(addr), port(port) {}

// to string "xxx.xxx.xxx.xxx:port"
std::string AddrPort::toString() const {
    return this->addr.to_string() + ":" + this->port.toString();
}

std::string AddrPort::toString(const Addr& addr, const Port& port, char separator) {
    return addr.to_string() + separator + port.toString();
}

} // namespace nano