// src/udp/udp_socket.cpp

#include "udp/udp_socket.hpp"

namespace nanonet {


// default constructor
UdpSocket::UdpSocket() :sockfd(-1), local({}), remote({}) {
    local.sin_family = AF_INET;
    sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd >= 0);
}


// destructor
UdpSocket::~UdpSocket() {}


// bind (addr : port)
void UdpSocket::bind(const Addr& addr, const Port& port) {
    local.sin_addr.s_addr = addr.hton();
    local.sin_port = port.hton();
    if (::bind(sockfd, (const sockaddr*)&local, sizeof(local)) < 0) {
        error << "[udp] bind: " << strerror(errno) << std::endl;
        exit(-1);
    }
}

void UdpSocket::bind(const AddrPort& addrPort) {
    this->bind(addrPort.getAddr(), addrPort.getPort());
}

void UdpSocket::bind(const std::string& ip, const Port& port) {
    this->bind(Addr(ip), port);
}

void UdpSocket::bind(const char* ip, const Port& port) {
    this->bind(Addr(ip), port);
}

void UdpSocket::bind(const Port& port) {
    this->bind(Addr(INADDR_ANY), port);
}


// set remote address & port
void UdpSocket::setRemote(const Addr& addr, const Port& port) {
    assert(sockfd >= 0);
    remote.sin_family = AF_INET;
    remote.sin_port = port.hton();
    remote.sin_addr.s_addr = addr.hton();
}

void UdpSocket::setRemote(const AddrPort& addrPort) {
    this->setRemote(addrPort.getAddr(), addrPort.getPort());
}

void UdpSocket::setRemote(const std::string& ip, const Port& port) {
    this->setRemote(Addr(ip), port);
}

void UdpSocket::setRemote(const char* ip, const Port& port) {
    this->setRemote(Addr(ip), port);
}

int UdpSocket::setReceiveTimeout(long seconds, long milliseconds) {
    struct timeval tm = {seconds, milliseconds * 1000};
    return this->setsockopt(SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(struct timeval));
}

// send data
int UdpSocket::send(const char* data, size_t datalen) const {
    if (sockfd < 0) {
        error << "[udp] socket is closed" << std::endl;
        exit(-1);
    }
    ssize_t ret = ::sendto(sockfd, data, datalen, 0,
        (const struct sockaddr*)&remote, sizeof(remote));
    if (ret < 0) {
        warn << "[udp] send: " << strerror(errno) << std::endl;
    } else if (ret == 0) {
        warn << "[udp] send: no data was sent";
    }
    return (int)ret;
}

int UdpSocket::send(const std::string& str) const {
    return this->send(str.c_str(), str.size());
}


// waiting to receive data from others
AddrPort UdpSocket::receive(char* buffer, size_t bufSize) const {
    if (sockfd < 0) {
        error << "[udp] socket is closed" << std::endl;
        exit(-1);
    }

    // receive data & write to buffer
    socklen_t socklen = sizeof(remote);
    ssize_t len = ::recvfrom(sockfd, buffer, bufSize, 0, (struct sockaddr*)&remote, &socklen);
    
    // received successfully
    if (len < 0) {
        warn << "[udp] receive: " << strerror(errno) << std::endl;
    } else if (len == 0) {
        warn << "[udp] receive: no data was received";
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
    } else {
        warn << "[udp] call close() repeatedly" << std::endl;
    }
}

int UdpSocket::setsockopt(int level, int optname, const void* optval, socklen_t optlen) const {
    return ::setsockopt(sockfd, level, optname, optval, optlen);
}

} // namespace nanonet