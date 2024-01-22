// addr.cpp

#include "addr.h"

namespace nano {

namespace {

// convert C string to addr_t
inline addr_t parse_(const char* addr) {
    if (Addr::is_valid(addr)) {
        return ::ntohl(inet_addr(addr));
    } else {
        return Addr::dns_query(addr).get();
    }
}

// compare addr_t and C string
inline bool equal_(const addr_t& addr, const char* other) {
    try {
        return addr == parse_(other);
    } catch (...) {
        return false;
    }
}

} // anonymous namespace

// constructor
Addr::Addr(addr_t val) : val_(val) {}

Addr::Addr(const char* addr) : val_(parse_(addr)) {}

Addr::Addr(const std::string& addr) : Addr(addr.c_str()) {}

// assign
Addr& Addr::operator=(addr_t other) {
    this->val_ = other;
    return *this;
}

Addr& Addr::operator=(const char* other) {
    this->val_ = parse_(other);
    return *this;
}

Addr& Addr::operator=(const std::string& other) {
    this->val_ = parse_(other.c_str());
    return *this;
}

bool Addr::operator==(addr_t other) const {
    return this->val_ == other;
}

bool Addr::operator!=(addr_t other) const {
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
addr_t Addr::net_order() const {
    return ::htonl(this->val_);
}

// setter & getter
addr_t Addr::get() const {
    return this->val_;
}

void Addr::set(addr_t val) {
    this->val_ = val;
}

// to string
std::string Addr::to_string() const {
    uint32_t val = static_cast<uint32_t>(::htonl(this->val_));
    return std::to_string(val & 0xFF) + '.' + std::to_string((val >> 8) & 0xFF) + '.'
        + std::to_string((val >> 16) & 0xFF) + '.' + std::to_string((val >> 24) & 0xFF);
}

// is valid ipv4 address
bool Addr::is_valid(const std::string& addr) {
    int count = 0, value = 0;
    char prev = '.';
    for (const char& c : addr) {
        if (c == '.') {
            if (prev == '.') return false;
            if (value > 255 || count == 3)
                return false;
            value = 0;
            ++count;
        } else if (c >= '0' && c <= '9') {
            value = value * 10 + (c & 0xF);
        } else {
            return false;
        }
        prev = c;
    }
    return (value <= 255) && (count == 3);
}

// DNS query
Addr Addr::dns_query(const char* domain, bool use_tcp) {
    addrinfo hints = {};
    // use tcp or udp?
    hints.ai_family = AF_INET;
    hints.ai_socktype = use_tcp ? SOCK_STREAM : SOCK_DGRAM;
    addrinfo* result;
    int status = getaddrinfo(domain, NULL, &hints, &result);
    assert_throw(status == 0,
        "[addr] getaddrinfo: ", gai_strerror(status));
    addr_t addr = INADDR_ANY;
    for (addrinfo* p = result; p; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            sockaddr_in* ipv4 = (sockaddr_in*)p->ai_addr;
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
