// port.h

#pragma once
#ifndef __PORT_H__
#define __PORT_H__

// C
#include <cstdint>

// C++
#include <string>

// linux
#include <netinet/in.h>

// nanonet
#include "nanonet/exception/nanoerr.h"

namespace nanonet {

class Port {

    // host byte order port
    in_port_t val;

    static in_port_t parse_(const std::string& port);

public:

    // constructor
    Port(in_port_t val = 0U);

    Port(const std::string& port);

    // to string
    std::string toString() const;

    static Port parsePort(const std::string& str);

    inline bool operator==(const Port& port) const {
        return this->val == port.val;
    }

    inline bool operator==(in_port_t val) const {
        return this->val == val;
    }

    inline bool operator!=(const Port& port) const {
        return this->val != port.val;
    }

    inline bool operator!=(in_port_t val) const {
        return this->val != val;
    }

    inline Port& operator=(in_port_t val) {
        this->setVal(val);
        return *this;
    }

    // host byte order -> network byte order
    inline uint16_t hton() const {
        return ::htons(this->val);
    }

    // getter & setter
    inline in_addr_t getVal() const {
        return this->val;
    }
    inline void setVal(in_addr_t val) {
        this->val = val;
    }

}; // class Port

} // namespace nanonet

#endif // __PORT_H__