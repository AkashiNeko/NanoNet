#pragma once
#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__

#include <string>
#include <utility>

#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "nanonet/nano_def.h"
#include "nanonet/addr_port.h"

namespace nanonet {

class UdpServer {
    std::string host;
    port_t port;
    int sockfd;
public:
    UdpServer(port_t port, std::string host = "");

    virtual ~UdpServer();

    std::pair<AddrPort, std::string> wait();

    ssize_t send(AddrPort hostPort, std::string messages);

}; // class UdpServer

} // namespace nanonet

#endif // __UDP_SERVER_H__