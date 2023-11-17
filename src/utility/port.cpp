// utility/port.cpp

#include "nanonet/utility/port.h"

namespace nanonet {

// constructor
Port::Port(in_port_t val) :val(val) {}

// to string
std::string Port::toString() const { return std::to_string(this->val); }

} // namespace nanonet