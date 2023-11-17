// src/tcp/socket.cpp

#include "tcp/socket.hpp"

namespace nanonet {


// default constructor
Socket::Socket() :sockfd(-1), remote({}) {
    sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error << "[tcp] socket: " << strerror(errno) << std::endl;
        exit(-1);
    }
    remote.sin_family = AF_INET;
}


// destructor
Socket::~Socket() {}


// connect to server
void Socket::connect(const Addr& addr, const Port& port) {
    if (sockfd < 0) {
        error << "[tcp] socket is closed" << std::endl;
        exit(-1);
    }
    remote.sin_addr.s_addr = addr.hton();
    remote.sin_port = port.hton();
    int connectResult = ::connect(sockfd, (const struct sockaddr*)&remote, sizeof(remote));
    if (connectResult < 0) {
        error << "[tcp] connect: " << strerror(errno) << std::endl;
        exit(-1);
    }
}

void Socket::connect(const AddrPort& addrPort) {
    this->connect(addrPort.getAddr(), addrPort.getPort());
}

void Socket::connect(const std::string& ip, const Port& port) {
    this->connect(Addr(ip), port);
}

void Socket::connect(const char* ip, const Port& port) {
    this->connect(Addr(ip), port);
}

void Socket::bind(const Addr& addr, const Port& port) const {
    sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = addr.hton();
    local.sin_port = port.hton();
    if (::bind(this->sockfd, (const struct sockaddr*)&local, sizeof(local)) < 0) {
        error << "[tcp] bind: " << strerror(errno) << std::endl;
        exit(-1);
    }
}

// close socket
void Socket::close() {
    if (sockfd >= 0) {
        ::close(sockfd);
        sockfd = -1;
    }
}

int Socket::setsockopt(int level, int optname, const void* optval, socklen_t optlen) const {
    return ::setsockopt(sockfd, level, optname, optval, optlen);
}

int Socket::setReceiveTimeout(long seconds, long milliseconds = 0) const {
    struct timeval tm = {seconds, milliseconds * 1000};
    return this->setsockopt(SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(struct timeval));
}

// send to remote
int Socket::send(const char* msg, size_t size) const {
    if (sockfd < 0) {
        error << "[tcp] socket is closed" << std::endl;
        exit(-1);
    }

    // send to remote
    int ret = ::send(sockfd, msg, size, 0);
    if (ret < 0) {
        warn << "[tcp] send: " << strerror(errno) << std::endl;
    } else if (ret == 0) {
        warn << "[tcp] send: no data was sent";
    }

    // returns the number of bytes sent
    return ret;
}

int Socket::send(std::string msg) const {
    return this->send(msg.c_str(), msg.size());
}

// receive from remote
int Socket::receive(char *buf, size_t buf_size) {
    if (sockfd < 0) {
        error << "[tcp] socket is closed" << std::endl;
        return -1;
    }

    // receive from remote
    ssize_t ret = ::recv(sockfd, buf, buf_size, 0);

    // truncate buffer
    if (ret >= 0 && ret < buf_size) buf[ret] = 0;

    // returns the number of bytes receive
    return (int)ret;
}


// get remote addrport
AddrPort Socket::getRemoteAddrPort() const {
    return AddrPort(::ntohl(remote.sin_addr.s_addr), ::ntohs(remote.sin_port));
}

} // namespace nanonet