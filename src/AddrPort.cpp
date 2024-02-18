// File:     src/AddrPort.cpp
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

#include "AddrPort.h"

namespace nano {

namespace {

// convert C string to Addr/Port
inline void parse_(const char* str, char separator, Addr& addr, Port& port) {
    const char* p = str;
    for (; *p && *p != ':'; ++p);
    assert_throw_nanoexcept(*p == ':',
        "[AddrPort] AddrPort(): Cannot be constructed from the string \'",
        str, "\'");
    addr = std::string(str, p - str);
    port = p + 1;
}

} // anonymous namespace

// constructor
AddrPort::AddrPort(const Addr& addr, const Port& port) noexcept
    : addr_(addr), port_(port) {}

AddrPort::AddrPort(std::string_view addrport, char separator) {
    parse_(addrport.data(), separator, this->addr_, this->port_);
}

// getter & setter
void AddrPort::addr(const Addr& addr) noexcept {
    this->addr_ = addr;
}

Addr AddrPort::addr() const noexcept {
    return this->addr_;
}

void AddrPort::port(const Port& port) noexcept {
    this->port_ = port;
}

Port AddrPort::port() const noexcept {
    return this->port_;
}

// to string
std::string AddrPort::to_string(char separator) const noexcept {
    return this->addr_.to_string() + separator + this->port_.to_string();
}

} // namespace nano
