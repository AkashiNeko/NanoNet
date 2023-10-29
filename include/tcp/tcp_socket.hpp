// tcp_socket.hpp

#pragma once
#ifndef __TCP_SOCKET_HPP__
#define __TCP_SOCKET_HPP__

// nanonet
#include "utility/addr_port.hpp"

// linux
#include <unistd.h>

// C
#include <cassert>

// C++
#include <string>

namespace nanonet {

class TCPSocket {
    
    // socket
    int sockfd;

    // remote
    struct sockaddr_in remote;

    // server socket
    friend class ServerSocket;

public:

    // default constructor
    TCPSocket() :sockfd(-1), remote({}) {
        sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        assert(sockfd >= 0);
        remote.sin_family = AF_INET;
    }


    // destructor (close fd)
    ~TCPSocket() {}


    // connect to server
    inline int connect(const Addr& addr, const Port& port) {
        assert(sockfd >= 0);
        remote.sin_addr.s_addr = addr.hton();
        remote.sin_port = port.hton();
        return ::connect(sockfd, (const struct sockaddr*)&remote, sizeof(remote));
    }

    inline int connect(const AddrPort& addrPort) {
        return this->connect(addrPort.addr, addrPort.port);
    }

    inline int connect(const std::string& ip, const Port& port) {
        return this->connect(Addr(ip), port);
    }

    inline int connect(const char* ip, const Port& port) {
        return this->connect(Addr(ip), port);
    }


    // receive from remote
    inline size_t receive(char *buf, size_t buf_size) const {
        // assert
        assert(sockfd >= 0);

        // receive from remote
        size_t ret = ::recv(sockfd, buf, buf_size, 0);

        // truncate buffer
        if (ret < buf_size) buf[ret] = 0;

        // returns the number of bytes receive
        return ret;
    }


    // send to remote
    inline ssize_t send(const char* msg, size_t size) const {
        assert(sockfd >= 0);

        // send to remote
        ssize_t ret = ::send(sockfd, msg, size, 0);
        assert(ret >= 0);

        // returns the number of bytes sent
        return ret;
    }

    inline ssize_t send(std::string msg) const {
        return this->send(msg.c_str(), msg.size());
    }


    // close socket
    inline void close() {
        if (sockfd >= 0) {
            ::close(sockfd);
            sockfd = -1;
        }
    }


    // get remote addrport
    inline AddrPort getRemoteAddrPort() const {
        return AddrPort(::ntohl(remote.sin_addr.s_addr), ::ntohs(remote.sin_port));
    }

}; // class TCPSocket

} // namespace nanonet

#endif // __SOCKET_HPP__