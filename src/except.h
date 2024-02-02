// File:     src/except.h
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
#ifndef NANONET_EXCEPT_H
#define NANONET_EXCEPT_H

// C
#include <cstring>

// C++
#include <string>
#include <exception>

namespace nano {

class NanoExcept : public std::exception {
    std::string except_msg_;
public:
    explicit NanoExcept(const std::string& msg) : except_msg_(msg) {}
    explicit NanoExcept(std::string&& msg) : except_msg_(std::move(msg)) {}
    virtual ~NanoExcept() override = default;
    virtual const char* what() const noexcept override {
        return except_msg_.c_str();
    }
};

// throw exceptions
#if __cplusplus >= 201703L

template <class ExceptType = NanoExcept, class ...Args>
inline void throw_except_(const Args&... args) {
    std::string s;
    ((s += args), ...);
    throw ExceptType(std::move(s));
}

#else // 201103L <= __cplusplus < 201703L

inline void append_string_(std::string&) {}

template <class T, class ...Args>
inline void append_string_(std::string& s, const T& arg, const Args&... args) {
    s += arg;
    append_string_(s, args...);
}

template <class ExceptType, class ...Args>
inline void throw_except_(const Args&... args) {
    std::string s;
    append_string_(s, args...);
    throw ExceptType(std::move(s));
}

#endif // __cplusplus

#define assert_throw_nanoexcept(condition, ...)    \
(static_cast<bool>(condition) ? void(0)            \
: throw_except_<NanoExcept>(__VA_ARGS__));         \

#ifdef __linux__

#define LAST_ERROR (std::strerror(errno))

#elif _WIN32

#include <WinSock2.h>
#include <Windows.h>

namespace {
    inline std::string WSAGetLastErrorMessage_() {
        LPSTR msg = nullptr;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER
            | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&msg, 0, nullptr);
        std::string result(msg);
        LocalFree(msg);
        return result;
    }
}

#define LAST_ERROR (WSAGetLastErrorMessage_())

#endif


} // namespace nano

#endif // NANONET_EXCEPT_H
