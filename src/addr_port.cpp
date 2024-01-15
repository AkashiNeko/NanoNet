// addr_port.cpp

#include "addr_port.h"

namespace nano {

// constructor
AddrPort::AddrPort(const Addr& addr, const Port& port)
    : addr_(addr), port_(port) {}

// getter & setter
void AddrPort::set_addr(const Addr& addr) {
    this->addr_ = addr;
}

void AddrPort::set_port(const Port& port) {
    this->port_ = port;
}

Addr AddrPort::get_addr() const {
    return this->addr_;
}

Port AddrPort::get_port() const {
    return this->port_;
}

// to string
std::string AddrPort::to_string() const {
    return this->addr_.to_string() + ":" + this->port_.to_string();
}

std::string AddrPort::to_string(const Addr& addr, const Port& port, char separator) {
    return addr.to_string() + separator + port.to_string();
}

// sockaddr_in -> AddrPort (static)
AddrPort AddrPort::to_addrport(sockaddr_in address) {
    return AddrPort(::ntohl(address.sin_addr.s_addr), ::ntohs(address.sin_port));
}

} // namespace nano
