// udp/udp_socket.cpp

#include "udp_socket.h"

namespace nano {


// default constructor
UdpSocket::UdpSocket() : local({}), remote({}) {
    local.sin_family = AF_INET;
    sock_fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    throw_except<UdpSocketExcept>(sock_fd_ >= 0,
        "[udp] create socket: ", strerror(errno));
}


// destructor
UdpSocket::~UdpSocket() {}


// bind (addr : port)
void UdpSocket::bind(const Addr& addr, const Port& port) {
    local.sin_addr.s_addr = addr.net_order();
    local.sin_port = port.net_order();
    int ret = ::bind(sock_fd_, (const sockaddr*)&local, sizeof(local));
    throw_except<UdpBindExcept>(ret >= 0, "[udp] bind \'",
        AddrPort::to_string(addr, port), "\': ", strerror(errno));
}

// set remote address & port
void UdpSocket::setRemote(const Addr& addr, const Port& port) {
    throw_except<UdpSocketClosedExcept>(sock_fd_ >= 0, "[udp] socket is closed");
    remote.sin_family = AF_INET;
    remote.sin_port = port.net_order();
    remote.sin_addr.s_addr = addr.net_order();
}

int UdpSocket::setReceiveTimeout(long seconds, long milliseconds) {
    struct timeval tm = {seconds, milliseconds * 1000};
    return this->setsockopt(SOL_SOCKET, SO_RCVTIMEO,
        &tm, sizeof(struct timeval));
}

// send data
int UdpSocket::send(const char* data, size_t datalen) const {
    throw_except<UdpSocketClosedExcept>(sock_fd_ >= 0, "[udp] socket is closed");
    ssize_t ret = ::sendto(sock_fd_, data, datalen, 0,
        (const struct sockaddr*)&remote, sizeof(remote));
    throw_except<UdpSendExcept>(ret >= 0, "[udp] send: ", strerror(errno));
    return static_cast<int>(ret);
}

// waiting to receive data from others
AddrPort UdpSocket::receive(char* buffer, size_t bufSize) const {
    throw_except<UdpSocketClosedExcept>(sock_fd_ >= 0, "[udp] socket is closed");

    // receive data & write to buffer
    socklen_t socklen = sizeof(remote);
    ssize_t len = ::recvfrom(sock_fd_, buffer, bufSize,
        0, (struct sockaddr*)&remote, &socklen);

    if (len < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            throw_except<UdpReceiveTimeoutExcept>(false, "[udp] receive timeout");
        } else {
            throw_except<UdpReceiveExcept>(false, "[udp] receive: ", strerror(errno));
        }
    }

    // truncate buffer
    if (len < bufSize) buffer[len] = 0;

    // return ip and port
    return AddrPort(::ntohl(remote.sin_addr.s_addr), ::ntohs(remote.sin_port));
}

} // namespace nano