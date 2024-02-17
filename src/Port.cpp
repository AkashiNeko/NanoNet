// File:     src/Port.cpp
// Author:   AkashiNeko
// Project:  NanoNet
// Github:   https://github.com/AkashiNeko/NanoNet/

/* Copyright AkashiNeko. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 *
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "Port.h"

namespace nano {

namespace {

// convert C string to port_t
inline port_t parse_(const char* port) {
    unsigned result = 0;
    for (const char* p = port; *p; ++p) {
        if (*p == ' ') continue;
        assert_throw_nanoexcept(*p >= '0' && *p <= '9',
            "[Port] Port(): The port string \'", port, "\' is invalied");
        result = result * 10 + (*p & 0xF);
        assert_throw_nanoexcept(result < 65536,
            "[Port] Port(): The port value \'", port, "\' is out of range");
    }
    return port_hton(static_cast<port_t>(result));
}

// compare port_t and C string
inline bool equal_(const port_t& port, const char* other) {
    try {
        return port == parse_(other);
    } catch (...) {
        return false;
    }
}

} // anonymous namespace

// constructor
Port::Port(port_t port) : val_(port_hton(port)) {}

Port::Port(std::string_view port) : val_(parse_(port.data())) {}

// assignment
Port& Port::operator=(port_t other) {
    this->val_ = other;
    return *this;
}

Port& Port::operator=(std::string_view other) {
    this->val_ = parse_(other.data());
    return *this;
}

bool Port::operator==(port_t other) const {
    return this->val_ == other;
}

bool Port::operator==(std::string_view other) const {
    return equal_(this->val_, other.data());
}

bool Port::operator!=(port_t other) const {
    return this->val_ != other;
}

bool Port::operator!=(std::string_view other) const {
    return !equal_(this->val_, other.data());
}

// getter & setter
port_t Port::get(bool net_order) const {
    return net_order ? this->val_ : port_ntoh(this->val_);
}

void Port::set(port_t val) {
    this->val_ = val;
}

// to string
std::string Port::to_string() const {
    return std::to_string(port_ntoh(this->val_));
}

} // namespace nano
