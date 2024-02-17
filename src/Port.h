// File:     src/Port.h
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
#ifndef NANONET_PORT_H
#define NANONET_PORT_H

// C++
#include <string>

// NanoNet
#include "net.h"

namespace nano {

class Port {

    // host byte order port
    port_t val_;

public:

    // ctor & dtor
    Port(port_t val = 0);
    Port(std::string_view port);

    Port(const Port&) = default;
    Port(Port&&) = default;
    virtual ~Port() = default;

    // assignment
    Port& operator=(const Port&) = default;
    Port& operator=(Port&&) = default;

    Port& operator=(port_t other);
    Port& operator=(std::string_view other);

    bool operator==(port_t other) const;
    bool operator==(std::string_view other) const;

    bool operator!=(port_t other) const;
    bool operator!=(std::string_view other) const;

    // getter & setter
    port_t get(bool net_order = true) const;
    void set(port_t val);

    // to string
    std::string to_string() const;

}; // class Port

} // namespace nano

#endif // NANONET_PORT_H
