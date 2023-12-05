// utility/port.cpp

#include "nanonet/utility/port.h"

namespace nanonet {

in_port_t Port::parse_(const std::string& str) {
    int port = 0;
    if (str.empty()) return 0;
    try {
        port = std::stoi(str);
    } catch (const std::exception& e) {
        throwError<ParsePortError>("[port] string \'",
            str, "\' to port: ", e.what());
    }
    if (port > 65535 || port < 0) {
        throwError<ParsePortError>("[port] port ", str, " is invalied");
    }
    return port;
}

// constructor
Port::Port(in_port_t val) :val(val) {}

Port::Port(const std::string& str) {
    this->val = parsePort(str).val;
}

// to string
std::string Port::toString() const { return std::to_string(this->val); }

} // namespace nanonet