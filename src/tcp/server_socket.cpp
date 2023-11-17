// tcp/server_socket.cpp

#include "nanonet/tcp/server_socket.h"

namespace nanonet {

// constructor (addr, port)
ServerSocket::ServerSocket(const Addr& addr, const Port& port, bool reuseAddr, int backlog) {
    // socket
    serverSockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (serverSockfd < 0) {
        error << "[tcp] socket: " << strerror(errno) << std::endl;
        exit(-1);
    }

    // set socket option
    if (reuseAddr) {
        const int on = 1;
        ::setsockopt(serverSockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    }

    // bind
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = addr.hton();
    local.sin_port = port.hton();
    if (::bind(serverSockfd, (const struct sockaddr*)&local, sizeof(local)) < 0) {
        error << "[tcp] bind: " << strerror(errno) << std::endl;
        exit(-1);
    }

    // listen
    if (::listen(serverSockfd, backlog) < 0) {
        error << "[tcp] listen: " << strerror(errno) << std::endl;
        exit(-1);
    } else {
        debug << "[tcp] server listening to \'" << addr.toString() << ":"
            << port.toString() << '\'' << std::endl;
    }
}

ServerSocket::ServerSocket(const std::string& ip, const Port& port, bool reuseAddr, int backlog)
    : ServerSocket(Addr(ip), port, reuseAddr, backlog) {}

ServerSocket::ServerSocket(const char* ip, const Port& port, bool reuseAddr, int backlog)
    : ServerSocket(Addr(ip), port, reuseAddr, backlog) {}

ServerSocket::ServerSocket(const Port& port, bool reuseAddr, int backlog)
    : ServerSocket(Addr(INADDR_ANY), port, reuseAddr, backlog) {}

// destructor
ServerSocket::~ServerSocket() {}

// accept from client
Socket ServerSocket::accept() {
    Socket socket;  // result socket
    socklen_t socklen = sizeof(socket.remote);
    // accept
    socket.sockfd = ::accept(serverSockfd, (struct sockaddr*)&socket.remote, &socklen);
    if (socket.sockfd < 0) {
        error << "[tcp] accept: " << strerror(errno) << std::endl;
        exit(-1);
    }
    return socket;
}

// close server socket
void ServerSocket::close() {
    if (serverSockfd >= 0) {
        ::close(serverSockfd);
        serverSockfd = -1;
    } else {
        warn << "[tcp] call close() repeatedly" << std::endl;
    }
}

} // namespace nanonet