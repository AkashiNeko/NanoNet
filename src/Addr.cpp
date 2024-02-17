// File:     src/Addr.cpp
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

#include "Addr.h"

namespace nano {

namespace {

// convert C string to addr_t
inline addr_t parse_(const char* addr) {
    if (is_valid_ipv4(addr)) {
        return inet_addr(addr);
    } else {
        try {
            return dns_query_single(addr);
        } catch (const NanoExcept& ne) {
            throw_except("[Addr] ", ne.what());
        }
    }
    return 0; // never
}

// compare addr_t and C string
inline bool equal_(const addr_t& addr, const char* other) {
    try {
        return addr == parse_(other);
    } catch (...) {
        return false;
    }
}

} // anonymous namespace

// constructor
Addr::Addr(addr_t val) : val_(addr_hton(val)) {}

Addr::Addr(std::string_view addr) : val_(parse_(addr.data())) {}

// assign
Addr& Addr::operator=(addr_t other) {
    this->val_ = other;
    return *this;
}

Addr& Addr::operator=(std::string_view addr) {
    this->val_ = parse_(addr.data());
    return *this;
}

bool Addr::operator==(addr_t other) const {
    return this->val_ == other;
}

bool Addr::operator==(std::string_view other) const {
    return equal_(this->val_, other.data());
}

bool Addr::operator!=(addr_t other) const {
    return this->val_ != other;
}

bool Addr::operator!=(std::string_view other) const {
    return !equal_(this->val_, other.data());
}

// setter & getter
addr_t Addr::get(bool net_order) const {
    return net_order ? this->val_ : addr_ntoh(this->val_);
}

void Addr::set(addr_t val) {
    this->val_ = val;
}

// to string
std::string Addr::to_string() const {
    return addr_ntos(this->val_);
}

}  // namespace nano
