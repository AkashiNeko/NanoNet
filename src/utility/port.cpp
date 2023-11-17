// src/utility/port.cpp

#include "utility/port.hpp"

namespace nanonet {

// constructor
Port::Port(in_port_t val) :val(val) {}

// to string
std::string Port::toString() const { return std::to_string(this->val); }

} // namespace nanonet