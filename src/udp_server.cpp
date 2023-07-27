#include "nanonet/udp_server.h"

namespace nanonet {

UdpServer::UdpServer(port_t port, std::string host) : port(port), host(host), sockfd(-1) {
    // create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(errno);
    }
    // reuse address
    struct sockaddr_in local;
    bzero(&local, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = host.empty() ? INADDR_ANY : inet_addr(host.c_str());
    // bind socket
    int bind_ret = bind(sockfd, (struct sockaddr*)&local, sizeof(local));
    if (bind_ret < 0) {
        perror("bind");
        exit(errno);
    }
}

UdpServer::~UdpServer() {
    // close socket
    if (sockfd >= 0)
        close(sockfd);
}

std::pair<HostPort, std::string> UdpServer::wait() {
    // wait for client
    char buf[4096];
    struct sockaddr_in remote;
    bzero(&remote, sizeof(remote));
    socklen_t socklen = sizeof(remote);
    ssize_t len = recvfrom(sockfd, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&remote, &socklen);
    buf[len] = '\0';
    host_t host = inet_addr(inet_ntoa(remote.sin_addr));
    port_t port = ntohs(remote.sin_port);
    // return host, port and message
    return {{host, port}, std::string(buf, len)};
}

ssize_t UdpServer::send(HostPort hostPort, std::string messages) {
    struct sockaddr_in remote;
    bzero(&remote, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_port = htons(hostPort.getPort());
    remote.sin_addr.s_addr = hostPort.getIP();
    return sendto(sockfd, messages.c_str(), messages.size(), 0, (const struct sockaddr*)&remote, sizeof(remote));
}

} // namespace nanonet