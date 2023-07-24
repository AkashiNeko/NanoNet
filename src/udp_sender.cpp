#include <utility>

#include "nanonet/udp_sender.h"

namespace nanonet {

UdpSender::UdpSender(std::string targetHost, port_t targetPort)
    : targetHost_(std::move(targetHost)), targetPort_(targetPort), server_({}) {
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd_ >= 0);
    server_.sin_family = AF_INET;
    server_.sin_port = htons(targetPort);
    server_.sin_addr.s_addr = inet_addr(targetHost.c_str());
}

UdpSender::~UdpSender() {
    if (sockfd_ >= 0) {
        ::close(sockfd_);
        sockfd_ = -1;
    }
}

ssize_t UdpSender::send(const std::string& message) {
    return sendto(sockfd_, message.c_str(), message.size(), 0, (struct sockaddr*)&server_, sizeof server_);
}

}  // namespace nanonet
