// requestor.hpp

#pragma once
#ifndef __REQUESTOR_HPP__
#define __REQUESTOR_HPP__

#include <thread>

#include "tcp/tcp_socket.hpp"
#include "http/http_request.hpp"


namespace nanonet {

const int BUF_SIZE = 4096;

class requestor {
    static Addr dnsServer;
public:
    static std::string send(http_request request, bool wait_respond = true) {
        std::string host = request.get_host();
        int slashPos = host.find(':');
        Port port = 80;
        if (slashPos != std::string::npos) {
            port.val = static_cast<in_port_t>(std::stoi(host.substr(slashPos + 1)));
            host = host.substr(0, slashPos);
        }
        Addr remote = Addr::isValid(host) ? Addr(host) : DNSQuery(dnsServer).resolve(host).addr;
        TCPSocket socket;
        socket.connect(remote, port);
        socket.send(request.to_string());
        if (wait_respond) {
            std::string ret;
            int recv_length = 0;
            
            do {
                char buffer[BUF_SIZE]{};
                recv_length = socket.receive(buffer, BUF_SIZE - 1);
                ret += buffer;
            } while (recv_length == BUF_SIZE - 1);
            return ret;
        }
        return {};
    }
}; // class requestor

Addr requestor::dnsServer = "114.114.114.114";

} // namespace nanonet

#endif // __REQUESTOR_HPP__