#include <utility>

#include "nanonet/udp_client.h"

namespace nanonet {

UdpClient::UdpClient(std::string targetHost, port_t targetPort)
    : targetHost_(targetHost), targetPort_(targetPort), sockfd_(-1), server_({}) {
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd_ >= 0);
    server_.sin_family = AF_INET;
    server_.sin_port = htons(targetPort);
    server_.sin_addr.s_addr = inet_addr(targetHost.c_str());
}

UdpClient::~UdpClient() {
    if (sockfd_ >= 0) {
        ::close(sockfd_);
        sockfd_ = -1;
    }
}

ssize_t UdpClient::send(const std::string message) {
    assert(sockfd_ >= 0);
    return sendto(sockfd_, message.c_str(), message.size(), 0, (struct sockaddr*)&server_, sizeof server_);
}

std::string UdpClient::wait() {
    assert(sockfd_ >= 0);
    char buf[4096];
    ssize_t n = recvfrom(sockfd_, buf, sizeof buf, 0, nullptr, nullptr);
    if (n > 0) {
        return std::string(buf, n);
    }
    return std::string();
}

}  // namespace nanonet
