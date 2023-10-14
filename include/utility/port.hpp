// port.hpp

#pragma once
#ifndef __PORT_HPP__
#define __PORT_HPP__

// C
#include <cstdint>

// C++
#include <string>

// linux
#include <netinet/in.h>

namespace nanonet {

struct Port {
public:

    // port
    in_port_t val;

public:

    // constructor
    Port(in_port_t val = 0U) :val(val) {}

    // to string
    std::string toString() const { return std::to_string(this->val); }

    // host byte order -> network byte order
    uint16_t hton() const { return ::htons(this->val); }

}; // struct Port

} // namespace nanonet

#endif // __PORT_HPP__