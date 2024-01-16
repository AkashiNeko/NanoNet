// udp/udp_socket.cpp

#include "udp_socket.h"

namespace nano {

namespace {

int receive_from_(int sock_fd, char* buf, size_t buf_size, AddrPort* addrport) {
    assert_throw(sock_fd >= 0, "[udp] socket is closed");
    sockaddr_in remote_addr;
    socklen_t socklen = sizeof(remote_addr);
    ssize_t len = ::recvfrom(sock_fd, buf, buf_size, 0, (sockaddr*)&remote_addr, &socklen);
    assert_throw(len >= 0 || errno == EAGAIN || errno == EWOULDBLOCK,
        "[udp] receive_from: ", std::strerror(errno));
    if (len >= 0 && len < buf_size) buf[len] = 0;
    if (addrport) {
        addrport->set_addr(::ntohl(remote_addr.sin_addr.s_addr));
        addrport->set_port(::ntohs(remote_addr.sin_port));
    }
    return len;
}

} // anonymous namespace

// constructor
UdpSocket::UdpSocket() : remote_({}), is_connected_(false) {
    remote_.sin_family = AF_INET;
    this->create_socket_(SOCK_DGRAM);
}

// send to the specified remote
int UdpSocket::send_to(const char* msg, size_t length, const AddrPort& remote) const {
    assert_throw(this->is_open(), "[udp] socket is closed");
    sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = remote.get_addr().net_order();
    remote_addr.sin_port = remote.get_port().net_order();
    ssize_t ret = ::sendto(socket_, msg, length, 0,
        (const sockaddr*)&remote_addr, sizeof(remote_addr));
    assert_throw(ret >= 0, "[udp] send_to: ", strerror(errno));
    return static_cast<int>(ret);
}

int UdpSocket::send_to(const std::string& msg, const AddrPort& remote) const {
    return send_to(msg.c_str(), msg.size(), remote);
}

// receive from the specified remote
int UdpSocket::receive_from(char* buf, size_t buf_size, AddrPort& addrport) const {
    return receive_from_(socket_, buf, buf_size, &addrport);
}

int UdpSocket::receive_from(char* buf, size_t buf_size) const {
    return receive_from_(socket_, buf, buf_size, nullptr);
}

// connect to remote
void UdpSocket::connect(const Addr& addr, const Port& port) {
    assert_throw(this->is_open(), "[udp] socket is closed");
    assert_throw(!is_connected_, "[udp] socket is connected");
    remote_.sin_addr.s_addr = addr.net_order();
    remote_.sin_port = port.net_order();

    // connect
    int ret = ::connect(socket_, (const sockaddr*)&remote_, sizeof(remote_));
    assert_throw(ret >= 0, "[udp] connect: ", std::strerror(errno));
    is_connected_ = true;

    socklen_t addr_len = sizeof(local_);
    getsockname(socket_, (sockaddr *)&local_, &addr_len);
}

// send to remote
int UdpSocket::send(const char* msg, size_t size) const {
    assert_throw(this->is_open(), "[udp] socket is closed");
    assert_throw(is_connected_, "[udp] socket is not connected");

    // send
    int len = ::send(socket_, msg, size, 0);
    assert_throw(len >= 0, "[udp] send:", std::strerror(errno));

    // returns the number of bytes sent
    return len;
}

int UdpSocket::send(const std::string& msg) const {
    return this->send(msg.c_str(), msg.size());
}

// receive from remote
int UdpSocket::receive(char* buf, size_t buf_size) const {
    assert_throw(this->is_open(), "[udp] socket is closed");
    assert_throw(is_connected_, "[udp] socket is not connected");

    ssize_t len = ::recv(socket_, buf, buf_size, 0);
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
    timeval tm = {ms / 1000, ms % 1000 * 1000};
    return set_option(SOL_SOCKET, SO_RCVTIMEO, tm);
}

// getter
AddrPort UdpSocket::get_remote() const {
    assert_throw(is_connected_, "[udp] socket is not connected");
    return AddrPort::to_addrport(remote_);
}

} // namespace nano
