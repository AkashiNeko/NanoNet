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
#include "except.h"

namespace nano {

class Port {

    // host byte order port
    in_port_t val_;

    static in_port_t parse_(const std::string& port);

public:

    // constructor
    Port(in_port_t val = 0U);

    Port(const std::string& port);

    // to string
    std::string toString() const;

    inline bool operator==(const Port& port) const {
        return this->val_ == port.val_;
    }

    inline bool operator==(in_port_t val) const {
        return this->val_ == val;
    }

    inline bool operator!=(const Port& port) const {
        return this->val_ != port.val_;
    }

    inline bool operator!=(in_port_t val) const {
        return this->val_ != val;
    }

    inline Port& operator=(in_port_t val) {
        this->setVal(val);
        return *this;
    }

    void parse(const std::string& val) {
        this->val_ = parse_(val);
    }

    // host byte order -> network byte order
    inline uint16_t hton() const {
        return ::htons(this->val_);
    }

    // getter & setter
    inline in_addr_t getVal() const {
        return this->val_;
    }
    inline void setVal(in_addr_t val) {
        this->val_ = val;
    }

}; // class Port

} // namespace nano

#endif // __PORT_H__