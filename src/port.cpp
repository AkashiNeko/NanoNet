// port.cpp

#include "port.h"

namespace nano {

namespace {

// convert C string to in_port_t
inline in_port_t parse_(const char* port) {
    unsigned result = 0;
    for (const char* p = port; *p; ++p) {
        assert_throw(*p >= '0' && *p <= '9',
            "[port] port ", port, " is invalied");
        result = result * 10 + (*p & 0xF);
        assert_throw(result < 65536,
            "[port] port ", port, " is out of range");
    }
    return static_cast<in_port_t>(result);
}

// compare in_port_t and C string
inline bool equal_(const in_port_t& port, const char* other) {
    try {
        return port == parse_(other);
    } catch (...) {
        return false;
    }
}

} // anonymous namespace

// constructor
Port::Port(in_port_t port)
    : val_(port) {}

Port::Port(const char* port)
    : val_(parse_(port)) {}

Port::Port(const std::string& port)
    : val_(parse_(port.c_str())) {}

// assign
Port& Port::operator=(in_port_t other) {
    this->val_ = other;
    return *this;
}

Port& Port::operator=(const char* other) {
    this->val_ = parse_(other);
    return *this;
}

Port& Port::operator=(const std::string& other) {
    this->val_ = parse_(other.c_str());
    return *this;
}

bool Port::operator==(in_port_t other) const {
    return this->val_ == other;
}

bool Port::operator!=(in_port_t other) const {
    return this->val_ != other;
}

bool Port::operator==(const char* other) const {
    return equal_(this->val_, other);
}

bool Port::operator!=(const char* other) const {
    return !equal_(this->val_, other);
}

bool Port::operator==(const std::string& other) const {
    return equal_(this->val_, other.c_str());
}

bool Port::operator!=(const std::string& other) const {
    return !equal_(this->val_, other.c_str());
}

// to network byte order
in_port_t Port::net_order() const {
    return ::htons(this->val_);
}

// getter & setter
in_addr_t Port::get() const {
    return this->val_;
}

void Port::set(in_addr_t val) {
    this->val_ = val;
}

// to string
std::string Port::to_string() const {
    return std::to_string(this->val_);
}

} // namespace nano
