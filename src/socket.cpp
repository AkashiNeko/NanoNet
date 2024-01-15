// socket.cpp

#include "socket.h"

namespace nano {

// constructor
Socket::Socket() : remote_() {
    remote_.sin_family = AF_INET;
    sock_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    throw_except<TcpSocketExcept>(sock_fd_ >= 0,
        "[tcp] create socket: ", std::strerror(errno));
}

void Socket::bind(const Addr& addr, const Port& port) {
    local_.sin_family = AF_INET;
    local_.sin_addr.s_addr = addr.net_order();
    local_.sin_port = port.net_order();
    int ret = ::bind(sock_fd_, (const struct sockaddr*)&local_, sizeof(local_));
    throw_except<TcpBindExcept>(ret >= 0, "[tcp] bind \'",
        AddrPort::to_string(addr, port), "\': ", std::strerror(errno));
}

// connect to server
void Socket::connect(const Addr& addr, const Port& port) {
    // set remote
    throw_except<TcpSocketClosedExcept>(sock_fd_ >= 0, "[tcp] socket is closed");
    remote_.sin_addr.s_addr = addr.net_order();
    remote_.sin_port = port.net_order();
    // connect to remote
    int ret = ::connect(sock_fd_, (const struct sockaddr*)&remote_, sizeof(remote_));
    throw_except<TcpConnectExcept>(ret >= 0, "[tcp] connect: ", std::strerror(errno));
    // get local
    socklen_t addr_len = sizeof(local_);
    getsockname(sock_fd_, (struct sockaddr *)&local_, &addr_len);
}

// send to remote
int Socket::send(const char* msg, size_t size) const {
    throw_except<TcpSocketClosedExcept>(sock_fd_ >= 0, "[tcp] socket is closed");

    // send to remote
    int ret = ::send(sock_fd_, msg, size, 0);
    throw_except<TcpSendExcept>(ret >= 0, "[tcp] send:", std::strerror(errno));

    // returns the number of bytes sent
    return ret;
}

int Socket::send(std::string msg) const {
    return this->send(msg.c_str(), msg.size());
}

// receive from remote
int Socket::receive(char* buf, size_t buf_size) {
    throw_except<TcpSocketClosedExcept>(sock_fd_ >= 0, "[tcp] socket is closed");
    ssize_t len = ::recv(sock_fd_, buf, buf_size, 0);
    if (len == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return -1;
        } else {
            throw_except<TcpReceiveExcept>(false, "[tcp] receive: ", std::strerror(errno));
        }
    }
    // truncate buffer
    if (len < buf_size) buf[len] = 0;
    return static_cast<int>(len);
}

bool Socket::recv_timeout(long ms) {
    struct timeval tm = {ms / 1000, ms % 1000 * 1000};
    return set_option(SOL_SOCKET, SO_RCVTIMEO, tm);
}

// get local
AddrPort Socket::get_local() const {
    return to_addrport_(local_);
}

// get remote
AddrPort Socket::get_remote() const {
    return to_addrport_(remote_);
}

} // namespace nano