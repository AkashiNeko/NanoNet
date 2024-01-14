// addr_port.cpp

#include "addr_port.h"

namespace nano {

// constructor
AddrPort::AddrPort(const Addr& addr, const Port& port)
    : addr(addr), port(port) {}

// getter & setter
void AddrPort::set_addr(const Addr& addr) {
    this->addr = addr;
}

void AddrPort::set_port(const Port& port) {
    this->port = port;
}

Addr AddrPort::get_addr() const {
    return this->addr;
}

Port AddrPort::get_port() const {
    return this->port;
}

// to string
std::string AddrPort::to_string() const {
    return this->addr.to_string() + ":" + this->port.to_string();
}

std::string AddrPort::to_string(const Addr& addr, const Port& port, char separator) {
    return addr.to_string() + separator + port.to_string();
}

} // namespace nano