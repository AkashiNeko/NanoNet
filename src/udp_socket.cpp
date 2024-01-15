// udp/udp_socket.cpp

#include "udp_socket.h"

namespace nano {


// default constructor
UdpSocket::UdpSocket() : remote_({}) {
    local_.sin_family = AF_INET;
    sock_fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    assert_throw(sock_fd_ >= 0,
        "[udp] create socket: ", strerror(errno));
}


// destructor
UdpSocket::~UdpSocket() {}

// bind (addr : port)
void UdpSocket::bind(const Addr& addr, const Port& port) {
    local_.sin_addr.s_addr = addr.net_order();
    local_.sin_port = port.net_order();
    int ret = ::bind(sock_fd_, (const sockaddr*)&local_, sizeof(local_));
    assert_throw(ret >= 0, "[udp] bind \'",
        AddrPort::to_string(addr, port), "\': ", strerror(errno));
}

// set remote address & port
void UdpSocket::setRemote(const Addr& addr, const Port& port) {
    assert_throw(sock_fd_ >= 0, "[udp] socket is closed");
    remote_.sin_family = AF_INET;
    remote_.sin_port = port.net_order();
    remote_.sin_addr.s_addr = addr.net_order();
}

// send data
int UdpSocket::send(const char* data, size_t datalen) const {
    assert_throw(sock_fd_ >= 0, "[udp] socket is closed");
    ssize_t ret = ::sendto(sock_fd_, data, datalen, 0,
        (const struct sockaddr*)&remote_, sizeof(remote_));
    assert_throw(ret >= 0, "[udp] send: ", strerror(errno));
    return static_cast<int>(ret);
}

// waiting to receive data from others
AddrPort UdpSocket::receive(char* buffer, size_t bufSize) const {
    assert_throw(sock_fd_ >= 0, "[udp] socket is closed");

    // receive data & write to buffer
    socklen_t socklen = sizeof(remote_);
    ssize_t len = ::recvfrom(sock_fd_, buffer, bufSize,
        0, (struct sockaddr*)&remote_, &socklen);

    if (len < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            assert_throw(false, "[udp] receive timeout");
        } else {
            assert_throw(false, "[udp] receive: ", strerror(errno));
        }
    }

    // truncate buffer
    if (len < bufSize) buffer[len] = 0;

    // return ip and port
    return AddrPort(::ntohl(remote_.sin_addr.s_addr), ::ntohs(remote_.sin_port));
}

} // namespace nano