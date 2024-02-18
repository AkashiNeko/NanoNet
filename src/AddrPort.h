// File:     src/AddrPort.h
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
#ifndef NANONET_ADDR_PORT_H
#define NANONET_ADDR_PORT_H

// NanoNet
#include "Addr.h"
#include "Port.h"

namespace nano {

class AddrPort {

    Addr addr_;
    Port port_;

public:

    // ctor & dtor
    AddrPort() = default;
    AddrPort(const Addr& addr, const Port& port) noexcept;
    AddrPort(std::string_view addrport, char separator = ':');

    AddrPort(const AddrPort&) = default;
    AddrPort(AddrPort&&) = default;
    virtual ~AddrPort() = default;

    // assignment
    AddrPort& operator=(const AddrPort&) = default;
    AddrPort& operator=(AddrPort&&) = default;

    // getter & setter
    Addr addr() const noexcept;
    void addr(const Addr& addr) noexcept;

    Port port() const noexcept;
    void port(const Port& port) noexcept;

    // to string
    std::string to_string(char separator = ':') const noexcept;

}; // class AddrPort

} // namespace nano

#endif // NANONET_ADDR_PORT_H
