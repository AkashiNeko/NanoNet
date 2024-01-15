// udp/udp_socket.cpp

#include "udp_socket.h"

namespace nano {

// constructor
UdpSocket::UdpSocket() : remote_({}), connected_(false) {
    remote_.sin_family = AF_INET;
    sock_fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    assert_throw(sock_fd_ >= 0,
        "[udp] create socket: ", strerror(errno));
}

// send to the specified remote
int UdpSocket::send_to(const char* msg, size_t length, const AddrPort& remote) const {
    assert_throw(sock_fd_ >= 0, "[udp] socket is closed");
    sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = remote.get_addr().net_order();
    remote_addr.sin_port = remote.get_port().net_order();
    ssize_t ret = ::sendto(sock_fd_, msg, length, 0,
        (const sockaddr*)&remote_addr, sizeof(remote_addr));
    assert_throw(ret >= 0, "[udp] send_to: ", strerror(errno));
    return static_cast<int>(ret);
}

int UdpSocket::send_to(const std::string& msg, const AddrPort& remote) const {
    return send_to(msg.c_str(), msg.size(), remote);
}

// receive from the specified remote
int UdpSocket::receive_from(char* buffer, size_t bufSize, AddrPort& addrport) const {
    assert_throw(sock_fd_ >= 0, "[udp] socket is closed");
    sockaddr_in remote_addr;
    socklen_t socklen = sizeof(remote_addr);
    ssize_t len = ::recvfrom(sock_fd_, buffer, bufSize, 0, (sockaddr*)&remote_addr, &socklen);
    if (len == -1) {
        assert_throw(errno == EAGAIN || errno == EWOULDBLOCK,
            "[udp] receive_from: ", std::strerror(errno));
        return -1;
    }
    // truncate buffer
    if (len < bufSize) buffer[len] = 0;
    return len;
}

// connect to remote
void UdpSocket::connect(const Addr& addr, const Port& port) {
    assert_throw(sock_fd_ >= 0, "[udp] socket is closed");
    assert_throw(!connected_, "[udp] socket is connected");
    remote_.sin_addr.s_addr = addr.net_order();
    remote_.sin_port = port.net_order();

    // connect
    int ret = ::connect(sock_fd_, (const struct sockaddr*)&remote_, sizeof(remote_));
    assert_throw(ret >= 0, "[udp] connect: ", std::strerror(errno));
    connected_ = true;

    socklen_t addr_len = sizeof(local_);
    getsockname(sock_fd_, (struct sockaddr *)&local_, &addr_len);
}

// send to remote
int UdpSocket::send(const char* msg, size_t size) const {
    assert_throw(sock_fd_ >= 0, "[udp] socket is closed");
    assert_throw(connected_, "[udp] socket is not connected");

    // send
    int len = ::send(sock_fd_, msg, size, 0);
    assert_throw(len >= 0, "[udp] send:", std::strerror(errno));

    // returns the number of bytes sent
    return len;
}

int UdpSocket::send(const std::string& msg) const {
    return this->send(msg.c_str(), msg.size());
}

// receive from remote
int UdpSocket::receive(char* buf, size_t buf_size) const {
    assert_throw(sock_fd_ >= 0, "[udp] socket is closed");
    assert_throw(connected_, "[udp] socket is not connected");

    ssize_t len = ::recv(sock_fd_, buf, buf_size, 0);
    if (len == -1) {
        assert_throw(errno == EAGAIN || errno == EWOULDBLOCK,
            "[udp] receive: ", std::strerror(errno));
        return -1;
    }
    // truncate buffer
    if (len < buf_size) buf[len] = 0;
    return static_cast<int>(len);
}

// set receive timeout
bool UdpSocket::recv_timeout(long ms) const {
    struct timeval tm = {ms / 1000, ms % 1000 * 1000};
    return set_option(SOL_SOCKET, SO_RCVTIMEO, tm);
}

// getter
AddrPort UdpSocket::get_remote() const {
    assert_throw(connected_, "[udp] socket is not connected");
    return AddrPort::to_addrport(remote_);
}

} // namespace nano