// socket.cpp

#include "socket.h"

namespace nano {

// constructor
Socket::Socket() : remote_() {
    remote_.sin_family = AF_INET;
    socket_ = ::socket(AF_INET, SOCK_STREAM, 0);
    assert_throw(this->is_open(),
        "[tcp] create socket: ", std::strerror(errno));
}

// connect to remote
void Socket::connect(const Addr& addr, const Port& port) {
    // set remote
    assert_throw(this->is_open(), "[tcp] socket is closed");
    remote_.sin_addr.s_addr = addr.net_order();
    remote_.sin_port = port.net_order();
    // connect to remote
    int ret = ::connect(socket_, (const sockaddr*)&remote_, sizeof(remote_));
    assert_throw(ret >= 0, "[tcp] connect: ", std::strerror(errno));
    // get local
    socklen_t addr_len = sizeof(local_);
    getsockname(socket_, (sockaddr *)&local_, &addr_len);
}

// send to remote
int Socket::send(const char* msg, size_t size) const {
    assert_throw(this->is_open(), "[tcp] socket is closed");

    // send to remote
    int len = ::send(socket_, msg, size, 0);
    assert_throw(len >= 0, "[tcp] send:", std::strerror(errno));

    // returns the number of bytes sent
    return len;
}

int Socket::send(const std::string msg) const {
    return this->send(msg.c_str(), msg.size());
}

// receive from remote
int Socket::receive(char* buf, size_t buf_size) const {
    assert_throw(this->is_open(), "[tcp] socket is closed");
    ssize_t len = ::recv(socket_, buf, buf_size, 0);
    if (len == -1) {
        assert_throw(errno == EAGAIN || errno == EWOULDBLOCK,
            "[tcp] receive: ", std::strerror(errno));
        return -1;
    }
    // truncate buffer
    if (len < buf_size) buf[len] = 0;
    return static_cast<int>(len);
}

bool Socket::recv_timeout(long ms) const {
    timeval tm = {ms / 1000, ms % 1000 * 1000};
    return set_option(SOL_SOCKET, SO_RCVTIMEO, tm);
}

// get remote
AddrPort Socket::get_remote() const {
    return AddrPort::to_addrport(remote_);
}

} // namespace nano
