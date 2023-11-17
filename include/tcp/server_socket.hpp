// server_socket.hpp

#pragma once
#ifndef __SERVER_SOCKET_HPP__
#define __SERVER_SOCKET_HPP__

// nanonet
#include "utility/addr_port.hpp"
#include "tcp/socket.hpp"

// linux
#include <unistd.h>

// C
#include <cassert>

// C++
#include <string>

namespace nanonet {

const int BACKLOG_QUEUE_SIZE = 20;

class ServerSocket {
    // server socket fd
    int serverSockfd;

    // local address
    struct sockaddr_in local;

public:
    // constructor (addr, port)
    ServerSocket(const Addr& addr, const Port& port, bool reuseAddr = true, int backlog = BACKLOG_QUEUE_SIZE);
    ServerSocket(const std::string& ip, const Port& port, bool reuseAddr = true, int backlog = BACKLOG_QUEUE_SIZE);
    ServerSocket(const char* ip, const Port& port, bool reuseAddr = true, int backlog = BACKLOG_QUEUE_SIZE);
    ServerSocket(const Port& port, bool reuseAddr = true, int backlog = BACKLOG_QUEUE_SIZE);

    // destructor
    ~ServerSocket();

    // accept from client
    Socket accept();

    // close server socket
    void close();

};  // class ServerSocket

}  // namespace nanonet

#endif  // __SERVER_SOCKET_HPP__