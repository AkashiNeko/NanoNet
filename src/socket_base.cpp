// socket_base.cpp

#include "socket_base.h"

namespace nano {

// constructor
SocketBase::SocketBase(int fd) : sock_fd_(fd) {
    local_.sin_family = AF_INET;
}

// file descriptor
void SocketBase::close() {
    if (sock_fd_ != -1) {
        ::close(sock_fd_);
        sock_fd_ = -1;
    }
}

bool SocketBase::is_open() const {
    return sock_fd_ != -1;
}

sock_t SocketBase::get_sock() const {
    return sock_fd_;
}

void SocketBase::bind(const Addr& addr, const Port& port) {
    local_.sin_addr.s_addr = addr.net_order();
    local_.sin_port = port.net_order();
    int ret = ::bind(sock_fd_, (const sockaddr*)&local_, sizeof(local_));
    assert_throw(ret >= 0, "[socket] bind \'",
        AddrPort::to_string(addr, port), "\': ", std::strerror(errno));
}

// get local
AddrPort SocketBase::get_local() const {
    assert_throw(sock_fd_ >= 0, "[socket] get_local: socket is closed");
    return AddrPort::to_addrport(local_);
}

} // namespace nano
