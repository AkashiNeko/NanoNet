// server_socket.cpp

#include "server_socket.h"

namespace nano {

// default constructor
ServerSocket::ServerSocket() {
    sock_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd_ < 0) {
        throw_except<TcpServerSocketExcept>(
                "[tcp] create socket: ", strerror(errno));
    }
    this->reuse_addr(true);
}

// constructor (addr, port)
ServerSocket::ServerSocket(const Addr& addr, const Port& port) {
    this->bind(addr, port);
}

// destructor
ServerSocket::~ServerSocket() {}

// set addr reuse
void ServerSocket::reuse_addr(bool enable) {
    // set socket option
    this->set_option(SOL_SOCKET , SO_REUSEADDR, enable);
}

// bind
void ServerSocket::bind(const Addr& addr, const Port& port) {
    local_.sin_family = AF_INET;
    local_.sin_addr.s_addr = addr.net_order();
    local_.sin_port = port.net_order();
    if (::bind(sock_fd_, (const struct sockaddr*)&local_, sizeof(local_)) < 0)
        throw_except<TcpBindExcept>("[tcp] bind: ", strerror(errno));
}

void ServerSocket::listen(int backlog) {
    if (::listen(sock_fd_, backlog) < 0)
        throw_except<TcpListenExcept>("[tcp] listen: ", strerror(errno));
    this->listening = true;
}

// accept a new connection
Socket ServerSocket::accept() {
    Socket socket;
    socklen_t socklen = sizeof(socket.remote_);
    int new_fd = ::accept(sock_fd_, (struct sockaddr*)&socket.remote_, &socklen);
    throw_except<TcpAcceptExcept>(new_fd >= 0, "[tcp] accept: ", strerror(errno));
    socket.sock_fd_ = new_fd;
    return socket;
}

} // namespace nano