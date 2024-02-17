// File:     src/Addr.h
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

#pragma once
#ifndef NANONET_ADDR_H
#define NANONET_ADDR_H

// C
#include <cstring>

// C++
#include <string>
#include <string_view>

// NanoNet
#include "net.h"

namespace nano {

class Addr {

    // host byte order addr (ipv4)
    addr_t val_;

public:

    // ctor & dtor
    Addr(addr_t val = 0);
    Addr(std::string_view addr);

    Addr(const Addr&) = default;
    Addr(Addr&&) = default;
    virtual ~Addr() = default;

    // assignment
    Addr& operator=(const Addr&) = default;
    Addr& operator=(Addr&&) = default;

    Addr& operator=(addr_t other);
    Addr& operator=(std::string_view addr);

    bool operator==(addr_t other) const;
    bool operator==(std::string_view other) const;

    bool operator!=(addr_t other) const;
    bool operator!=(std::string_view other) const;

    // setter & getter
    addr_t get(bool net_order = true) const;
    void set(addr_t val);

    // to string
    std::string to_string() const;

};  // class Addr

}  // namespace nano

#endif  // NANONET_ADDR_H
