// udp_packet.hpp

#pragma once
#ifndef __UDP_PACKET_HPP__
#define __UDP_PACKET_HPP__

// UDP Packet: 

//  1) IP Address
//  2) Port number
//  3) Data and length

// nanonet
#include "nanonet/addr_port.hpp"

// C
#include <cstring>

namespace nanonet {

class UdpPacket {

    // {addr, port}
    AddrPort addrPort_;
    // data length
    size_t dataSize_;
    // data
    char* data_;

public:

    // construct by string
    UdpPacket(std::string str) :addrPort_({}), dataSize_(str.size()) {
        data_ = str.empty() ? nullptr : new char[dataSize_ + 1];
        strncpy(data_, str.c_str(), dataSize_);
        data_[dataSize_] = '\0';
    }

    // construct by char[]
    UdpPacket(const char *data, size_t datalen) :addrPort_({}) {
        dataSize_ = datalen == -1 ? strlen(data) : datalen;
        data_ = new char[dataSize_ + 1];
        strncpy(data_, data, dataSize_);
        this->data_[dataSize_] = '\0';
    }

    // destructor
    ~UdpPacket() {
        if (data_ != nullptr) {
            delete[] data_;
            data_ = nullptr;
        }
    }

    // get addrport
    inline AddrPort getAddrPort() const {
        return addrPort_;
    }

    // get data length
    inline size_t size() const {
        return dataSize_;
    }

    // get data
    inline const char* data() const {
        return data_;
    }

    // set addrport
    inline void setAddrPort(AddrPort newAddrPort) {
        addrPort_ = newAddrPort;
    }

}; // UdpPacket

} // namespace nanonet

#endif // __UDP_PACKET_HPP__