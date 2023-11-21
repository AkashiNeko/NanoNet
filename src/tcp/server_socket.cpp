// tcp/server_socket.cpp

#include "nanonet/tcp/server_socket.h"
#include "nanonet/exception/nanoerr.h"

namespace nanonet {

// default constructor
ServerSocket::ServerSocket() {
    serverfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0) {
        throwError<TcpServerSocketError>(
            "[tcp] create socket: ", strerror(errno));
    }
    this->setReuseAddr(true);
}

// constructor (addr, port)
ServerSocket::ServerSocket(const Addr& addr, const Port& port) {
    this->bind(addr, port);
}

// destructor
ServerSocket::~ServerSocket() {}

// set addr reuse
void ServerSocket::setReuseAddr(bool reuseAddr) {
    // set socket option
    if (reuseAddr) {
        const int on = 1;
        ::setsockopt(serverfd, SOL_SOCKET,
            SO_REUSEADDR, &on, sizeof(on));
    }
}

// bind
void ServerSocket::bind(const Addr& addr, const Port& port) {
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = addr.hton();
    local.sin_port = port.hton();
    if (::bind(serverfd, (const struct sockaddr*)&local, sizeof(local)) < 0)
        throwError<TcpBindError>("[tcp] bind: ", strerror(errno));
}

void ServerSocket::listen(int backlog) {
    if (::listen(serverfd, backlog) < 0)
        throwError<TcpListenError>("[tcp] listen: ", strerror(errno));
    this->listening = true;
}

// accept
Socket ServerSocket::accept() {
    Socket socket; // result socket
    // accept
    socklen_t socklen = sizeof(socket.remote);
    int fd = ::accept(serverfd, (struct sockaddr*)&socket.remote, &socklen);
    fd < 0 && throwError<TcpAcceptError>("[tcp] accept: ", strerror(errno));
    socket.sockfd = fd;
    return socket;
}

// close server socket
void ServerSocket::close() {
    if (serverfd >= 0) {
        ::close(serverfd);
        serverfd = -1;
        listening = false;
    }
}

} // namespace nanonet