// except.h

#pragma once
#ifndef NANONET_EXCEPT_H_
#define NANONET_EXCEPT_H_

#include <string>
#include <exception>

// // define exception classes
// #define DEFINE_NANOEXCEPT_CLASS(DerivedClass, BaseClass)                    \
// class DerivedClass : public BaseClass {                                     \
// public:                                                                     \
//     explicit DerivedClass(const std::string& msg) : BaseClass(msg) {}       \
//     explicit DerivedClass(std::string&& msg) : BaseClass(std::move(msg)) {} \
//     virtual ~DerivedClass() override = default;                             \
// };                                                                          \

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
inline void throw_except(bool condition, const Args&... args) {
    if (condition) return;
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

template <class ExceptType = NanoExcept, class ...Args>
inline void assert_throw(bool condition, const Args&... args) {
    if (condition) return;
    std::string s;
    append_string_(s, args...);
    throw ExceptType(std::move(s));
}

#endif // __cplusplus

} // namespace nano

#endif // NANONET_EXCEPT_H_
