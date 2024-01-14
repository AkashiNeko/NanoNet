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
// | | AddrError |  | TcpError | |
// | '-----------'  '----------' |
// | .-----------.  .----------. |
// | | PortError |  | UdpError | |
// | '-----------'  '----------' |
// + - - - - - - - - - - - - - - +

// Addr
DEFINE_NANOEXCEPT_CLASS(AddrError, NanoExcept)
DEFINE_NANOEXCEPT_CLASS(AddrNTOPError, AddrError)
DEFINE_NANOEXCEPT_CLASS(GetAddrInfoError, AddrError)

// Port
DEFINE_NANOEXCEPT_CLASS(PortError, NanoExcept)
DEFINE_NANOEXCEPT_CLASS(ParsePortError, PortError)

// TCP
DEFINE_NANOEXCEPT_CLASS(TcpError, NanoExcept)
DEFINE_NANOEXCEPT_CLASS(TcpSocketError, TcpError)
DEFINE_NANOEXCEPT_CLASS(TcpServerSocketError, TcpError)
DEFINE_NANOEXCEPT_CLASS(TcpBindError, TcpError)
DEFINE_NANOEXCEPT_CLASS(TcpListenError, TcpError)
DEFINE_NANOEXCEPT_CLASS(TcpAcceptError, TcpError)
DEFINE_NANOEXCEPT_CLASS(TcpConnectError, TcpError)
DEFINE_NANOEXCEPT_CLASS(TcpSendError, TcpError)
DEFINE_NANOEXCEPT_CLASS(TcpReceiveError, TcpError)
DEFINE_NANOEXCEPT_CLASS(TcpReceiveTimeoutError, TcpError)
DEFINE_NANOEXCEPT_CLASS(TcpSocketClosedError, TcpError)

// UDP
DEFINE_NANOEXCEPT_CLASS(UdpError, NanoExcept)
DEFINE_NANOEXCEPT_CLASS(UdpSocketError, UdpError)
DEFINE_NANOEXCEPT_CLASS(UdpBindError, UdpError)
DEFINE_NANOEXCEPT_CLASS(UdpSendError, UdpError)
DEFINE_NANOEXCEPT_CLASS(UdpReceiveError, UdpError)
DEFINE_NANOEXCEPT_CLASS(UdpReceiveTimeoutError, UdpError)
DEFINE_NANOEXCEPT_CLASS(UdpSocketClosedError, UdpError)

#if __cplusplus >= 201703L

template <class ExceptType, class ...Args>
inline bool throw_except(const Args&... args) {
    std::string s;
    ((s += args), ...);
    throw ExceptType(std::move(s));
    return false;
}

#else // __cplusplus < 201703L

inline static void append_string_(std::string&) {}

template <class T, class ...Args>
inline static void append_string_(std::string& s, const T& arg, const Args&... args) {
    s += arg;
    append_string_(s, args...);
}

template <class ExceptType, class ...Args>
inline bool throw_except(const Args&... args) {
    std::string s;
    append_string_(s, args...);
    throw ExceptType(std::move(s));
}

#endif // __cplusplus

} // namespace nano

#endif // NANONET_EXCEPT_H_