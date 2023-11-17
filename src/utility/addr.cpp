// utility/addr.cpp

#include "nanonet/utility/addr.h"

namespace nanonet {

// constructor
Addr::Addr(in_addr_t val) : val(val) {}

Addr::Addr(const char* host) {
    if (isValid(host)) {
        this->val = ::ntohl(inet_addr(host));
    } else {
        this->val = getAddrByName(host).val;
        debug << "[addr] getaddrinfo " << host << " -> " << this->toString() << std::endl;
    }
}

Addr::Addr(const std::string& ip)
    : Addr(ip.c_str()) {}

// to string "xx.xx.xx.xx"
std::string Addr::toString() const {
    struct in_addr inAddr;
    inAddr.s_addr = ::htonl(this->val);
    char strAddr[INET_ADDRSTRLEN];
    const char* result = ::inet_ntop(AF_INET, &(inAddr.s_addr), strAddr, sizeof(strAddr));
    return result == nullptr ? std::string() : strAddr;
}

// is valid ipv4 address
bool Addr::isValid(const std::string& ip) {
    std::regex pattern("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    return std::regex_match(ip, pattern);
}

// DNS query
Addr Addr::getAddrByName(const char* domain, bool useTCP) {
    struct addrinfo hints = {}, *result, *p;
    int status;
    hints.ai_family = AF_INET;
    hints.ai_socktype = useTCP ? SOCK_STREAM : SOCK_DGRAM;
    status = getaddrinfo(domain, NULL, &hints, &result);
    if (status != 0) {
        error << "[addr] getaddrinfo: " << gai_strerror(status) << std::endl;
        exit(-1);
    }
    in_addr_t addr = INADDR_ANY;
    for (p = result; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = ipv4->sin_addr.s_addr;
            break;
        }
    }
    freeaddrinfo(result);
    return Addr(::htonl(addr));
}

Addr Addr::getAddrByName(const std::string& domain, bool useTCP) {
    return Addr::getAddrByName(domain.c_str(), useTCP);
}

}  // namespace nanonet