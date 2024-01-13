// addr.cpp

#include "addr.h"
#include "except.h"

namespace nano {

static in_addr_t parse_(const char* addr) {
    if (Addr::is_valid(addr)) {
        return ::ntohl(inet_addr(addr));
    } else {
        return Addr::dns_query(addr).get();
    }
}

// constructor
Addr::Addr(in_addr_t val) : val_(val) {}

Addr::Addr(const char* addr) : val_(parse_(addr)) {}

Addr::Addr(const std::string& addr) : Addr(addr.c_str()) {}

Addr& Addr::operator=(in_addr_t val) {
    this->val_ = val;
    return *this;
}

Addr& Addr::operator=(const char* val) {
    this->val_ = parse_(val);
    return *this;
}

Addr& Addr::operator=(const std::string& val) {
    this->val_ = parse_(val.c_str());
    return *this;
}

bool Addr::operator==(in_addr_t val) {
    return this->val_ == val;
}
bool Addr::operator!=(in_addr_t val) {
    return this->val_ != val;
}
bool Addr::operator==(const char* val) {
    try {
        return this->val_ == parse_(val);
    } catch (...) {
        return false;
    }
}
bool Addr::operator!=(const char* val) {
    return !((*this) == val);
}
bool Addr::operator==(const std::string& val) {
    try {
        return this->val_ == parse_(val.c_str());
    } catch (...) {
        return false;
    }
}
bool Addr::operator!=(const std::string& val) {
    return !((*this) == val);
}

// to network byte order
in_addr_t Addr::net_order() const {
    return ::htonl(this->val_);
}

// setter & getter
void Addr::set(in_addr_t val) {
    this->val_ = val;
}

in_addr_t Addr::get() const {
    return this->val_;
}

// to string
std::string Addr::to_string() const {
    struct in_addr inAddr;
    inAddr.s_addr = ::htonl(this->val_);
    char strAddr[INET_ADDRSTRLEN];
    const char* result = ::inet_ntop(AF_INET, &(inAddr.s_addr), strAddr, sizeof(strAddr));
    !result && throwError<AddrNTOPError>("[addr] inet_ntop: ", strerror(errno));
    return strAddr;
}

// is valid ipv4 address
bool Addr::is_valid(const std::string& ip) {
    std::regex pattern("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    return std::regex_match(ip, pattern);
}

// DNS query
Addr Addr::dns_query(const char* domain, bool use_tcp) {
    struct addrinfo hints = {};
    // use tcp or udp?
    hints.ai_family = AF_INET;
    hints.ai_socktype = use_tcp ? SOCK_STREAM : SOCK_DGRAM;
    struct addrinfo* result;
    int status = getaddrinfo(domain, NULL, &hints, &result);
    status != 0 && throwError<GetAddrInfoError>(
        "[addr] getaddrinfo: ", gai_strerror(status));
    in_addr_t addr = INADDR_ANY;
    for (struct addrinfo* p = result; p; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = ipv4->sin_addr.s_addr;
            break;
        }
    }
    freeaddrinfo(result);
    return Addr(::htonl(addr));
}
Addr Addr::dns_query(const std::string& domain, bool use_tcp) {
    return dns_query(domain.c_str(), use_tcp);
}

}  // namespace nano