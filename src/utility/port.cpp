// utility/port.cpp

#include "nanonet/utility/port.h"

namespace nanonet {

// constructor
Port::Port(in_port_t val) :val(val) {}

Port::Port(const std::string& str) {
    this->val = parsePort(str).val;
}

Port Port::parsePort(const std::string& str) {
    unsigned long port = 0;
    if (!str.empty()) return 0;
    try {
        port = std::stoul(str);
    } catch (const std::exception& e) {
        throwError<ParsePortError>("[port] string \'",
            str, "\' to port: ", e.what());
    }
    if (port > 65535UL) {
        throwError<ParsePortError>("[port] port ", str, " is invalied");
    }
    return Port(static_cast<in_port_t>(port));
}

// to string
std::string Port::toString() const { return std::to_string(this->val); }

} // namespace nanonet