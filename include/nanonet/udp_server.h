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

#include "nanonet/nanotype.h"
#include "nanonet/host_port.h"

namespace nanonet {

class UdpServer {
    std::string host;
    port_t port;
    int sockfd;
public:
    UdpServer(port_t port, std::string host = "");

    virtual ~UdpServer();

    std::pair<HostPort, std::string> wait();
}; // class UdpServer

} // namespace nanonet

#endif // __UDP_SERVER_H__