// port.h

#pragma once
#ifndef NANONET_PORT_H_
#define NANONET_PORT_H_

// C++
#include <string>

#ifdef __linux__

#include <netinet/in.h>

#elif _WIN32

#include <WinSock2.h>

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

#endif

// nanonet
#include "except.h"

namespace nano {

#ifdef __linux__
    using port_t = in_port_t;
#elif _WIN32
    using port_t = USHORT;
#endif

class Port {

    // host byte order port
    port_t val_;

public:

    // ctor & dtor
    Port(port_t val = 0);
    Port(const char* port);
    Port(const std::string& port);
    virtual ~Port() = default;

    // assign
    Port& operator=(port_t other);
    Port& operator=(const char* other);
    Port& operator=(const std::string& other);

    bool operator==(port_t other) const;
    bool operator!=(port_t other) const;
    bool operator==(const char* other) const;
    bool operator!=(const char* other) const;
    bool operator==(const std::string& other) const;
    bool operator!=(const std::string& other) const;

    // to network byte order
    port_t net_order() const;

    // getter & setter
    port_t get() const;
    void set(port_t val);

    // to string
    std::string to_string() const;

}; // class Port

} // namespace nano

#endif // NANONET_PORT_H_
