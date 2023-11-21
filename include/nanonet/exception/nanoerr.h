// nanoerr.h

#pragma once
#ifndef __NANOERR_H__
#define __NANOERR_H__

#include <string>
#include <exception>

// define exception classes
#define DEFINE_NANOERR_CLASS(DerivedClass, BaseClass)                       \
class DerivedClass : public BaseClass {                                     \
public:                                                                     \
    explicit DerivedClass(const std::string& msg) : BaseClass(msg) {}       \
    explicit DerivedClass(std::string&& msg) : BaseClass(std::move(msg)) {} \
    virtual ~DerivedClass() override = default;                             \
};                                                                          \

namespace nanonet {

class Error : public std::exception {
    std::string errmsg;
public:
    explicit Error(const std::string& msg) : errmsg(msg) {}
    explicit Error(std::string&& msg) : errmsg(std::move(msg)) {}
    // template <class T>
    // explicit Error(T&& msg) :errmsg(std::forward<T>(msg)) {}
    virtual ~Error() override = default;

    virtual const char* what() const noexcept override {
        return errmsg.c_str();
    }
};

// Addr
DEFINE_NANOERR_CLASS(AddrError, Error)
DEFINE_NANOERR_CLASS(AddrNTOPError, AddrError)
DEFINE_NANOERR_CLASS(GetAddrInfoError, AddrError)


// TCP
DEFINE_NANOERR_CLASS(TcpError, Error)

DEFINE_NANOERR_CLASS(TcpSocketError, TcpError)
DEFINE_NANOERR_CLASS(TcpServerSocketError, TcpError)
DEFINE_NANOERR_CLASS(TcpBindError, TcpError)
DEFINE_NANOERR_CLASS(TcpListenError, TcpError)
DEFINE_NANOERR_CLASS(TcpAcceptError, TcpError)
DEFINE_NANOERR_CLASS(TcpConnectError, TcpError)
DEFINE_NANOERR_CLASS(TcpSendError, TcpError)
DEFINE_NANOERR_CLASS(TcpReceiveError, TcpError)
DEFINE_NANOERR_CLASS(TcpSocketClosedError, TcpError)


// UDP
DEFINE_NANOERR_CLASS(UdpError, Error)

DEFINE_NANOERR_CLASS(UdpSocketError, UdpError)
DEFINE_NANOERR_CLASS(UdpBindError, UdpError)
DEFINE_NANOERR_CLASS(UdpSendError, UdpError)
DEFINE_NANOERR_CLASS(UdpReceiveError, UdpError)
DEFINE_NANOERR_CLASS(UdpSocketClosedError, UdpError)


// HTTP
DEFINE_NANOERR_CLASS(HttpError, Error)


#if __cplusplus >= 201703L

template <class ErrType, class ...Args>
inline bool throwError(const Args&... args) {
    std::string s;
    ((s += args), ...);
    throw ErrType(std::move(s));
    return false;
}

#else // __cplusplus < 201703L

inline void _append_string(std::string& s) {}

template <class T, class ...Args>
inline void _append_string(std::string& s, const T& arg, const Args&... args) {
    s += arg;
    _append_string(s, args...);
}

template <class ErrType, class ...Args>
inline bool throwError(const Args&... args) {
    std::string s;
    _append_string(s, args...);
    throw ErrType(std::move(s));
    return false;
}

#endif // __cplusplus

} // namespace nanonet

#endif // __NANOERR_H__