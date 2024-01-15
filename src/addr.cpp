// addr.cpp

#include "addr.h"

namespace nano {

namespace {

// convert C string to in_addr_t
inline in_addr_t parse_(const char* addr) {
    if (Addr::is_valid(addr)) {
        return ::ntohl(inet_addr(addr));
    } else {
        return Addr::dns_query(addr).get();
    }
}

// compare in_addr_t and C string
inline bool equal_(const in_addr_t& addr, const char* other) {
    try {
        return addr == parse_(other);
    } catch (...) {
        return false;
    }
}

} // anonymous namespace

// constructor
Addr::Addr(in_addr_t val) : val_(val) {}

Addr::Addr(const char* addr) : val_(parse_(addr)) {}

Addr::Addr(const std::string& addr) : Addr(addr.c_str()) {}

Addr& Addr::operator=(in_addr_t other) {
    this->val_ = other;
    return *this;
}

// assign
Addr& Addr::operator=(const char* other) {
    this->val_ = parse_(other);
    return *this;
}

Addr& Addr::operator=(const std::string& other) {
    this->val_ = parse_(other.c_str());
    return *this;
}

bool Addr::operator==(in_addr_t other) const {
    return this->val_ == other;
}

bool Addr::operator!=(in_addr_t other) const {
    return this->val_ != other;
}

bool Addr::operator==(const char* other) const {
    return equal_(this->val_, other);
}

bool Addr::operator!=(const char* other) const {
    return !equal_(this->val_, other);
}

bool Addr::operator==(const std::string& other) const {
    return equal_(this->val_, other.c_str());
}

bool Addr::operator!=(const std::string& other) const {
    return !equal_(this->val_, other.c_str());
}

// to network byte order
in_addr_t Addr::net_order() const {
    return ::htonl(this->val_);
}

// setter & getter
in_addr_t Addr::get() const {
    return this->val_;
}

void Addr::set(in_addr_t val) {
    this->val_ = val;
}

// to string
std::string Addr::to_string() const {
    struct in_addr inAddr;
    inAddr.s_addr = ::htonl(this->val_);
    char strAddr[INET_ADDRSTRLEN];
    const char* result = ::inet_ntop(AF_INET, &(inAddr.s_addr), strAddr, sizeof(strAddr));
    assert_throw(result != nullptr, "[addr] inet_ntop: ", strerror(errno));
    return strAddr;
}

// is valid ipv4 address
bool Addr::is_valid(const std::string& addr) {
    int count = 0, value = 0;
    for (const char& c : addr) {
        if (c == '.') {
            if (value > 255 || count == 3)
                return false;
            value = 0;
            ++count;
        } else if (c >= '0' && c <= '9') {
            value = value * 10 + (c & 0xF);
        } else {
            return false;
        }
    }
    return (value <= 255) && (count == 3);
}

// DNS query
Addr Addr::dns_query(const char* domain, bool use_tcp) {
    struct addrinfo hints = {};
    // use tcp or udp?
    hints.ai_family = AF_INET;
    hints.ai_socktype = use_tcp ? SOCK_STREAM : SOCK_DGRAM;
    struct addrinfo* result;
    int status = getaddrinfo(domain, NULL, &hints, &result);
    assert_throw(status == 0,
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
