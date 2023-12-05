// utility/addr.cpp

#include "nanonet/utility/addr.h"
#include "nanonet/exception/nanoerr.h"

namespace nanonet {

in_addr_t Addr::parse_(const char* addr) {
    if (isValid(addr)) {
        return ::ntohl(inet_addr(addr));
    } else {
        return getAddrByName(addr).val_;
    }
}

// constructor
Addr::Addr(in_addr_t val)
    : val_(val) {}

Addr::Addr(const char* addr)
    :val_(parse_(addr)) {}

// to string "xx.xx.xx.xx"
std::string Addr::toString() const {
    struct in_addr inAddr;
    inAddr.s_addr = ::htonl(this->val_);
    char strAddr[INET_ADDRSTRLEN];
    const char* result = ::inet_ntop(AF_INET, &(inAddr.s_addr), strAddr, sizeof(strAddr));
    !result && throwError<AddrNTOPError>("[addr] inet_ntop: ", strerror(errno));
    return strAddr;
}

// is valid ipv4 address
bool Addr::isValid(const std::string& ip) {
    std::regex pattern("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    return std::regex_match(ip, pattern);
}

// DNS query
Addr Addr::getAddrByName(const char* domain, bool useTcp) {
    struct addrinfo hints = {}, *result, *p;
    hints.ai_family = AF_INET;
    // use tcp or udp?
    hints.ai_socktype = useTcp ? SOCK_STREAM : SOCK_DGRAM;
    int status = getaddrinfo(domain, NULL, &hints, &result);
    status != 0 && throwError<GetAddrInfoError>(
        "[addr] getaddrinfo: ", gai_strerror(status));
    in_addr_t addr = INADDR_ANY;
    for (p = result; p; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = ipv4->sin_addr.s_addr;
            break;
        }
    }
    freeaddrinfo(result);
    return Addr(::htonl(addr));
}

}  // namespace nanonet