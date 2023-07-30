#include "nanonet/udp_packet.h"

namespace nanonet {

// construct by string
UdpPacket::UdpPacket(std::string str) :addrPort_({}), dataSize_(str.size()) {
    data_ = str.empty() ? nullptr : new char[dataSize_ + 1];
    strncpy(data_, str.c_str(), dataSize_);
    data_[dataSize_] = '\0';
}

// construct by char[]
UdpPacket::UdpPacket(const char *data, size_t datalen) :addrPort_({}) {
    dataSize_ = datalen == -1 ? strlen(data) : datalen;
    data_ = new char[dataSize_ + 1];
    strncpy(data_, data, dataSize_);
    this->data_[dataSize_] = '\0';
}

// destructor
UdpPacket::~UdpPacket() {
    if (data_ != nullptr) {
        delete[] data_;
        data_ = nullptr;
    }
}

}; // namespace nanonet
