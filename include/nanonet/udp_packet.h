// udp_packet.h

#pragma once
#ifndef __UDP_PACKET_H__
#define __UDP_PACKET_H__

// UDP Packet: 

//  1) IP Address
//  2) Port number
//  3) Data and length

#include "nanonet/addr_port.h"
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
    UdpPacket(std::string str);

    // construct by char[]
    UdpPacket(const char* data, size_t datalen = -1);

    // destructor
    ~UdpPacket();

    // get addrport
    inline AddrPort addrPort() const { return addrPort_; }

    // get addr
    inline addr_t addr() const { return addrPort_.getAddr(); }

    // get port
    inline port_t port() const { return addrPort_.getPort(); }
    
    // get data length
    inline size_t size() const { return dataSize_; }

    // get data
    inline const char* data() const { return data_; }

    // set addrport
    inline void setAddrPort(AddrPort newAddrPort) { addrPort_ = newAddrPort; }
};

} // namespace nanonet

#endif // __UDP_PACKET_H__