// udp/udp_socket.cpp

#include "udp_socket.h"

namespace nano {


// default constructor
UdpSocket::UdpSocket() :sockfd(-1), local({}), remote({}) {
    local.sin_family = AF_INET;
    sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
    sockfd < 0 && throwError<UdpSocketError>(
        "[udp] create socket: ", strerror(errno));
}


// destructor
UdpSocket::~UdpSocket() {}


// bind (addr : port)
void UdpSocket::bind(const Addr& addr, const Port& port) {
    local.sin_addr.s_addr = addr.net_order();
    local.sin_port = port.net_order();
    if (::bind(sockfd, (const sockaddr*)&local, sizeof(local)) < 0) {
        throwError<UdpBindError>("[udp] bind \'",
            AddrPort::to_string(addr, port), "\': ", strerror(errno));
    }
}

// set remote address & port
void UdpSocket::setRemote(const Addr& addr, const Port& port) {
    assert(sockfd >= 0);
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
    sockfd < 0 && throwError<UdpSocketClosedError>("[udp] socket is closed");
    ssize_t ret = ::sendto(sockfd, data, datalen, 0,
        (const struct sockaddr*)&remote, sizeof(remote));
    ret < 0 && throwError<UdpSendError>("[udp] send: ") + strerror(errno);
    return static_cast<int>(ret);
}

// waiting to receive data from others
AddrPort UdpSocket::receive(char* buffer, size_t bufSize) const {
    sockfd < 0 && throwError<UdpSocketClosedError>("[udp] socket is closed");

    // receive data & write to buffer
    socklen_t socklen = sizeof(remote);
    ssize_t len = ::recvfrom(sockfd, buffer, bufSize,
        0, (struct sockaddr*)&remote, &socklen);

    if (len < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            throwError<UdpReceiveTimeoutError>("[udp] receive timeout");
        } else {
            throwError<UdpReceiveError>("[udp] receive: ", strerror(errno));
        }
    }

    // truncate buffer
    if (len < bufSize) buffer[len] = 0;

    // return ip and port
    return AddrPort(::ntohl(remote.sin_addr.s_addr), ::ntohs(remote.sin_port));
}


// close socket fd
void UdpSocket::close() {
    if (sockfd >= 0) {
        ::close(sockfd);
        sockfd = -1;
    }
}

int UdpSocket::setsockopt(int level, int optname, const void* optval, socklen_t optlen) const {
    return ::setsockopt(sockfd, level, optname, optval, optlen);
}

} // namespace nano