// server_socket.cpp

#include "server_socket.h"

namespace nano {

// constructor
ServerSocket::ServerSocket() {
    sock_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    assert_throw(sock_fd_ >= 0,
        "[tcp] create socket: ", strerror(errno));
    this->reuse_addr(true);
}

ServerSocket::ServerSocket(const Addr& addr, const Port& port) : SocketBase() {
    this->bind(addr, port);
}

// listen
void ServerSocket::listen(int backlog) {
    int ret = ::listen(sock_fd_, backlog);
    assert_throw(ret >= 0, "[tcp] listen: ", strerror(errno));
}

// accept a new connection
Socket ServerSocket::accept() {
    Socket socket;
    socklen_t socklen = sizeof(socket.remote_);
    int new_fd = ::accept(sock_fd_, (sockaddr*)&socket.remote_, &socklen);
    assert_throw(new_fd >= 0, "[tcp] accept: ", strerror(errno));
    socket.sock_fd_ = new_fd;
    socket.local_ = this->local_;
    return socket;
}

// set addr reuse
bool ServerSocket::reuse_addr(bool enable) {
    return this->set_option(SOL_SOCKET, SO_REUSEADDR, (int)enable);
}

} // namespace nano