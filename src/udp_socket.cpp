#include "nanonet/udp_socket.h"

namespace nanonet {

// construct
UdpSocket::UdpSocket()
    : sockfd_(-1), local_({}), target_({}) {
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd_ >= 0);
}

UdpSocket::UdpSocket(port_t port) :UdpSocket() {
    local_.sin_family = AF_INET;
    local_.sin_addr.s_addr = INADDR_ANY;
    local_.sin_port = htons(port);
    int bind_ret = bind(sockfd_, (const sockaddr*)&local_, sizeof(local_));
    assert(bind_ret >= 0);
}

// set target
void UdpSocket::target(AddrPort addrPort) {
    target_.sin_family = AF_INET;
    target_.sin_port = addrPort.getNetPort();
    target_.sin_addr.s_addr = addrPort.getNetAddr();
}

int UdpSocket::send(UdpPacket packet) {
    std::string ip = inet_ntoa(target_.sin_addr);
    auto port = ntohs(target_.sin_port);
    return (int)sendto(sockfd_, packet.data(), packet.size(),
        0, (const struct sockaddr*)&target_, sizeof(target_));
}

UdpPacket UdpSocket::receive() {
    assert(sockfd_ >= 0);
    char buf[4096];
    struct sockaddr_in remote{};
    socklen_t socklen = sizeof(remote);
    ssize_t len = recvfrom(sockfd_, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&remote, &socklen);
    assert(len >= 0);
    buf[len] = '\0';
    AddrPort addrPort(remote.sin_addr.s_addr, remote.sin_port);
    UdpPacket packet(buf, len);
    packet.setAddrPort(addrPort);
    return packet;
}

};  // namespace nanonet
