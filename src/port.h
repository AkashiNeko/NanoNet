// port.h

#pragma once
#ifndef NANONET_PORT_H_
#define NANONET_PORT_H_

// C
#include <cstdint>

// C++
#include <string>

// Linux
#include <netinet/in.h>

// nanonet
#include "except.h"

namespace nano {

class Port {

    // host byte order port
    in_port_t val_;

public:

    // ctor & dtor
    Port(in_port_t val = 0);
    Port(const char* port);
    Port(const std::string& port);
    virtual ~Port() = default;

    // assign
    Port& operator=(in_port_t other);
    Port& operator=(const char* other);
    Port& operator=(const std::string& other);

    bool operator==(in_port_t other) const;
    bool operator!=(in_port_t other) const;
    bool operator==(const char* other) const;
    bool operator!=(const char* other) const;
    bool operator==(const std::string& other) const;
    bool operator!=(const std::string& other) const;

    // to network byte order
    in_port_t net_order() const;

    // getter & setter
    in_addr_t get() const;
    void set(in_addr_t val);

    // to string
    std::string to_string() const;

}; // class Port

} // namespace nano

#endif // NANONET_PORT_H_
