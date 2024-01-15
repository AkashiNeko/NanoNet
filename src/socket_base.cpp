// socket_base.cpp

#include "socket_base.h"

namespace nano {

// constructor
SocketBase::SocketBase(int fd) : sock_fd_(fd) {}

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

int SocketBase::get_fd() const {
    return sock_fd_;
}

// sockaddr_in -> AddrPort (static)
AddrPort to_addrport_(sockaddr_in address) {
    return AddrPort(::ntohl(address.sin_addr.s_addr), ::ntohs(address.sin_port));
}

} // namespace nano
