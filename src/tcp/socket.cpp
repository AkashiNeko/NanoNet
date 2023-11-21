// tcp/socket.cpp

#include "nanonet/tcp/socket.h"

namespace nanonet {

// default constructor
Socket::Socket() :sockfd(-1), remote({}) {
    remote.sin_family = AF_INET;
    sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    sockfd < 0 && throwError<TcpSocketError>(
        "[tcp] create socket: ", strerror(errno));
}

// destructor
Socket::~Socket() {}

// connect to server
void Socket::connect(const Addr& addr, const Port& port) {
    sockfd < 0 && throwError<TcpSocketClosedError>("[tcp] socket is closed");
    remote.sin_addr.s_addr = addr.hton();
    remote.sin_port = port.hton();
    int ret = ::connect(sockfd, (const struct sockaddr*)&remote, sizeof(remote));
    ret < 0 && throwError<TcpConnectError>("[tcp] connect: ", strerror(errno));
}

void Socket::bind(const Addr& addr, const Port& port) {
    sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = addr.hton();
    local.sin_port = port.hton();
    if (::bind(this->sockfd, (const struct sockaddr*)&local, sizeof(local)) < 0) {
        throwError<TcpBindError>("[tcp] bind \'",
            AddrPort::toString(addr, port), "\': ", strerror(errno));
    }
}

// close socket
void Socket::close() {
    if (sockfd >= 0) {
        ::close(sockfd);
        sockfd = -1;
    }
}

int Socket::setSocketOption(int level, int optname, const void* optval, socklen_t optlen) {
    return ::setsockopt(sockfd, level, optname, optval, optlen);
}

int Socket::setReceiveTimeout(long seconds, long milliseconds) {
    struct timeval tm = {seconds, milliseconds * 1000};
    return this->setSocketOption(SOL_SOCKET,
        SO_RCVTIMEO, &tm, sizeof(struct timeval));
}

// send to remote
int Socket::send(const char* msg, size_t size) const {
    sockfd < 0 && throwError<TcpSocketClosedError>("[tcp] socket is closed");

    // send to remote
    int ret = ::send(sockfd, msg, size, 0);
    ret < 0 && throwError<TcpSendError>("[tcp] send:", strerror(errno));

    // returns the number of bytes sent
    return ret;
}

int Socket::send(std::string msg) const {
    return this->send(msg.c_str(), msg.size());
}

// receive from remote
int Socket::receive(char *buf, size_t buf_size) {
    sockfd < 0 && throwError<TcpSocketClosedError>("[tcp] socket is closed");

    // receive from remote
    ssize_t len = ::recv(sockfd, buf, buf_size, 0);

    len < 0 && throwError<TcpReceiveError>("[tcp] receive: ", strerror(errno));
    // truncate buffer
    if (len < buf_size) buf[len] = 0;

    // returns the number of bytes receive
    return static_cast<int>(len);
}

// get remote addrport
AddrPort Socket::getRemoteAddrPort() const {
    return AddrPort(::ntohl(remote.sin_addr.s_addr), ::ntohs(remote.sin_port));
}

} // namespace nanonet