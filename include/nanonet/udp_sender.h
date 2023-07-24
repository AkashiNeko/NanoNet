#pragma once
#ifndef __UDP_SENDER_H__
#define __UDP_SENDER_H__

#include <string>
#include <strings.h>
#include <string.h>

#include "nanonet/nanotype.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cassert>

namespace nanonet {

class UdpSender {
    std::string targetHost_;
    port_t targetPort_;
    int sockfd_;
    struct sockaddr_in server_{};
public:
    UdpSender(std::string targetHost, port_t targetPort);
    virtual ~UdpSender();
    ssize_t send(const std::string message);

}; // class UdpSender

} // namespace nanonet

#endif // __UDP_SENDER_H__