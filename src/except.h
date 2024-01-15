// except.h

#pragma once
#ifndef NANONET_EXCEPT_H_
#define NANONET_EXCEPT_H_

#include <string>
#include <exception>

// define exception classes
#define DEFINE_NANOEXCEPT_CLASS(DerivedClass, BaseClass)                       \
class DerivedClass : public BaseClass {                                     \
public:                                                                     \
    explicit DerivedClass(const std::string& msg) : BaseClass(msg) {}       \
    explicit DerivedClass(std::string&& msg) : BaseClass(std::move(msg)) {} \
    virtual ~DerivedClass() override = default;                             \
};                                                                          \

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

// .------------.    .----------------.
// | NanoExcept | -> | std::exception |
// '------------'    '----------------'
//     A
//     |
// + - - - - - - - - - - - - - - +
// | .-----------.  .----------. |
// | | AddrExcept |  | TcpExcept | |
// | '-----------'  '----------' |
// | .-----------.  .----------. |
// | | PortExcept |  | UdpExcept | |
// | '-----------'  '----------' |
// + - - - - - - - - - - - - - - +

// Addr
DEFINE_NANOEXCEPT_CLASS(AddrExcept, NanoExcept)
DEFINE_NANOEXCEPT_CLASS(AddrNTOPExcept, AddrExcept)
DEFINE_NANOEXCEPT_CLASS(GetAddrInfoExcept, AddrExcept)

// Port
DEFINE_NANOEXCEPT_CLASS(PortExcept, NanoExcept)
DEFINE_NANOEXCEPT_CLASS(ParsePortExcept, PortExcept)

// TCP
DEFINE_NANOEXCEPT_CLASS(TcpExcept, NanoExcept)
DEFINE_NANOEXCEPT_CLASS(TcpSocketExcept, TcpExcept)
DEFINE_NANOEXCEPT_CLASS(TcpServerSocketExcept, TcpExcept)
DEFINE_NANOEXCEPT_CLASS(TcpBindExcept, TcpExcept)
DEFINE_NANOEXCEPT_CLASS(TcpListenExcept, TcpExcept)
DEFINE_NANOEXCEPT_CLASS(TcpAcceptExcept, TcpExcept)
DEFINE_NANOEXCEPT_CLASS(TcpConnectExcept, TcpExcept)
DEFINE_NANOEXCEPT_CLASS(TcpSendExcept, TcpExcept)
DEFINE_NANOEXCEPT_CLASS(TcpReceiveExcept, TcpExcept)
DEFINE_NANOEXCEPT_CLASS(TcpReceiveTimeoutExcept, TcpExcept)
DEFINE_NANOEXCEPT_CLASS(TcpSocketClosedExcept, TcpExcept)

// UDP
DEFINE_NANOEXCEPT_CLASS(UdpExcept, NanoExcept)
DEFINE_NANOEXCEPT_CLASS(UdpSocketExcept, UdpExcept)
DEFINE_NANOEXCEPT_CLASS(UdpBindExcept, UdpExcept)
DEFINE_NANOEXCEPT_CLASS(UdpSendExcept, UdpExcept)
DEFINE_NANOEXCEPT_CLASS(UdpReceiveExcept, UdpExcept)
DEFINE_NANOEXCEPT_CLASS(UdpReceiveTimeoutExcept, UdpExcept)
DEFINE_NANOEXCEPT_CLASS(UdpSocketClosedExcept, UdpExcept)

// throw exceptions
#if __cplusplus >= 201703L

template <class ExceptType, class ...Args>
inline void throw_except(bool condition, const Args&... args) {
    if (condition) return;
    std::string s;
    ((s += args), ...);
    throw ExceptType(std::move(s));
}

#else // 201103L <= __cplusplus < 201703L

inline static void append_string_(std::string&) {}

template <class T, class ...Args>
inline static void append_string_(std::string& s, const T& arg, const Args&... args) {
    s += arg;
    append_string_(s, args...);
}

template <class ExceptType, class ...Args>
inline void throw_except(bool condition, const Args&... args) {
    if (condition) return;
    std::string s;
    append_string_(s, args...);
    throw ExceptType(std::move(s));
}

#endif // __cplusplus

} // namespace nano

#endif // NANONET_EXCEPT_H_
