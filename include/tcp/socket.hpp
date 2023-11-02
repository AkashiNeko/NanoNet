// socket.hpp

#pragma once
#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

// linux
#include <unistd.h>

// C
#include <cassert>

// C++
#include <string>

// nanonet
#include "utility/addr_port.hpp"

namespace nanonet {

class Socket {
    
    // socket
    int sockfd;

    // remote
    struct sockaddr_in remote;

    // server socket
    friend class ServerSocket;

public:

    // default constructor
    Socket() :sockfd(-1), remote({}) {
        sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        assert(sockfd >= 0);
        remote.sin_family = AF_INET;
    }


    // destructor (close fd)
    ~Socket() {}


    // connect to server
    inline void connect(const Addr& addr, const Port& port) {
        assert(sockfd >= 0);
        remote.sin_addr.s_addr = addr.hton();
        remote.sin_port = port.hton();
        int connectResult = ::connect(sockfd, (const struct sockaddr*)&remote, sizeof(remote));
        if (connectResult >= 0) {
            Log::debug << "[tcp] connect to " << addr.toString() << ":" << port.toString() << " success" << std::endl;
        } else {
            Log::error << "[tcp] connect: " << strerror(errno) << std::endl;
            exit(-1);
        }
    }

    inline void connect(const AddrPort& addrPort) {
        this->connect(addrPort.addr, addrPort.port);
    }

    inline void connect(const std::string& ip, const Port& port) {
        this->connect(Addr(ip), port);
    }

    inline void connect(const char* ip, const Port& port) {
        this->connect(Addr(ip), port);
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

    int setReceiveTimeout(long seconds, long milliseconds) {
        struct timeval tm = {seconds, milliseconds * 1000};
        return this->setsockopt(SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(struct timeval));
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
            Log::debug << "[tcp] connection closed" << std::endl;
        }
    }


    // get remote addrport
    inline AddrPort getRemoteAddrPort() const {
        return AddrPort(::ntohl(remote.sin_addr.s_addr), ::ntohs(remote.sin_port));
    }

    inline int setsockopt(int level, int optname, const void* optval, socklen_t optlen) {
        return ::setsockopt(sockfd, level, optname, optval, optlen);
    }

}; // class Socket

} // namespace nanonet

#endif // __SOCKET_HPP__