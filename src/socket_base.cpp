// socket_base.cpp

#include "socket_base.h"

#ifdef _WIN32
namespace { // init wsa
class WSAInit {
public:
    WSAInit() {
        WSADATA wsa;
        int result = WSAStartup(MAKEWORD(2, 2), &wsa);
        nano::assert_throw(result == 0, "WSAStartup failed");
    }
    ~WSAInit() {
        WSACleanup();
    }
}__wsa_initializer_; // RAII
} // anonymous namespace
#endif

namespace nano {

// constructor
SocketBase::SocketBase() {
#ifdef __linux__
    socket_ = -1;
#elif _WIN32
    sock_open_ = false;
#endif
    local_.sin_family = AF_INET;
}

void SocketBase::create_socket_(int type) {
    socket_ = ::socket(AF_INET, type, 0);
#if _WIN32
    sock_open_ = socket_ != INVALID_SOCKET;
#endif
    assert_throw(is_open(), "[socket] create socket faild");
}

// file descriptor
void SocketBase::close() {
#ifdef __linux__
    if (socket_ != -1) {
        ::close(socket_);
        socket_ = -1;
    }
#elif _WIN32
    if (sock_open_) {
        closesocket(socket_);
        sock_open_ = false;
    }
#endif
}

bool SocketBase::is_open() const {
#ifdef __linux__
    return socket_ != -1;
#elif _WIN32
    return sock_open_;
#endif
}

sock_t SocketBase::get_sock() const {
    return socket_;
}

void SocketBase::bind(const Addr& addr, const Port& port) {
    local_.sin_addr.s_addr = addr.net_order();
    local_.sin_port = port.net_order();
    int ret = ::bind(socket_, (const sockaddr*)&local_, sizeof(local_));
    assert_throw(ret >= 0, "[socket] bind \'",
        AddrPort::to_string(addr, port), "\': ", strerror(errno));
}

// get local
AddrPort SocketBase::get_local() const {
    assert_throw(this->is_open(), "[socket] get_local: socket is closed");
    return AddrPort::to_addrport(local_);
}

} // namespace nano
