// utility/addr_port.cpp

#include "nanonet/utility/addr_port.h"

namespace nanonet {

// construct from address and port (host byte order)
AddrPort::AddrPort(const Addr& addr, const Port& port) :addr(addr), port(port) {}

// to string "xxx.xxx.xxx.xxx:port"
std::string AddrPort::toString() const {
    return this->addr.toString() + ":" + this->port.toString();
}

std::string AddrPort::toString(const Addr& addr, const Port& port, char separator) {
    return addr.toString() + separator + port.toString();
}

} // namespace nanonet