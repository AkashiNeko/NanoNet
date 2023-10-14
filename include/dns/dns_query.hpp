// dns_query.hpp

#ifndef __DNS_QUERY_HPP__
#define __DNS_QUERY_HPP__

// C
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

// C++
#include <string>
#include <vector>
#include <unordered_map>

// nanonet
#include "utility/addr_port.hpp"
#include "udp/udp_socket.hpp"

#define DNS_HOST			0x01
#define DNS_CNAME			0x05

namespace nanonet {

struct DNSQueryResult {
    std::string aname;
    Addr addr;
    int ttl;
};

class DNSQuery {

    Addr serverAddr;
    Port serverPort;
    
    static std::unordered_map<std::string, Addr> cache;

private:

    struct Header {
        unsigned short id;              // Session Identifier
        unsigned short flags;           // Flags
        unsigned short questions;       // Number of Questions
        unsigned short answer;          // Number of Answer Resource Records
        unsigned short authority;       // Number of Authority Resource Records
        unsigned short additional;      // Number of Additional Resource Records
    };

    struct Queries {
        int length;
        unsigned short qtype;
        unsigned short qclass;
        char* name;
    };

public:



    DNSQuery(const Addr& serverAddr, const Port& serverPort = 53)
        :serverAddr(serverAddr), serverPort(serverPort) {}


    inline std::vector<DNSQueryResult> resolveAll(const char* domain) {
        std::vector<DNSQueryResult> results;
        dnsQuery(domain, results);
        return results;
    }

    inline std::vector<DNSQueryResult> resolveAll(const std::string& domain) {
        std::vector<DNSQueryResult> results;
        dnsQuery(domain.c_str(), results);
        return results;
    }

    inline DNSQueryResult resolve(const char* domain) {
        std::vector<DNSQueryResult> results;
        dnsQuery(domain, results);
        return *min_element(results.begin(), results.end(), [](auto& a, auto& b) {
            return a.ttl < b.ttl;
        });
    }

    inline DNSQueryResult resolve(const std::string& domain) {
        return resolve(domain.c_str());
    }

private:

    void dnsQuery(const char* domain, std::vector<DNSQueryResult>& results) {
        UDPSocket socket;
        socket.setRemote(serverAddr, serverPort);
        Header header{};
        header.id = random();
        header.flags = htons(0x0100);
        header.questions = htons(1);
        struct Queries question{};
        createQueries(&question, domain);
        char request[1024]{};
        int length = buildRequest(header, question, request, sizeof request);
        auto sendret = socket.send(request, length);
        char response[1024]{};
        socket.receive(response, 1023);
        socket.close();
        parseResponse(response, results);
    }

    int createQueries(Queries* question, const char* hostname) {
        memset(question, 0, sizeof(struct Queries));
        question->name = (char*)malloc(strlen(hostname) + 2);
        question->length = strlen(hostname) + 2;
        question->qtype = htons(1);
        question->qclass = htons(1);
        const char delim[2] = ".";
        char* qname = question->name;
        char* hostname_dup = strdup(hostname); // strdup -> malloc
        char* token = strtok(hostname_dup, delim);
        while (token != nullptr) {
            size_t len = strlen(token);
            *qname++ = len;
            strncpy((char*)qname, token, len + 1);
            qname += len;
            token = strtok(NULL, delim);
        }
        free(hostname_dup);
        return 0;
    }

    int buildRequest(Header header, Queries question, char* request, int rlen) {
        int offset = 0;
        memset(request, 0, rlen);
        memcpy(request, &header, sizeof(struct Header));
        offset = sizeof(Header);
        memcpy(request + offset, question.name, question.length);
        offset += question.length;
        memcpy(request + offset, &question.qtype, sizeof(question.qtype));
        offset += sizeof(question.qtype);
        memcpy(request + offset, &question.qclass, sizeof(question.qclass));
        offset += sizeof(question.qclass);
        return offset;
    }


    void parseName(char* chunk, const char* ptr, char* out, int* len) {
        char* pos = out + (*len);
        while (int flag = (int)ptr[0]) {
            if (flag & 0xC0) {
                ptr = chunk + (int)ptr[1];
                parseName(chunk, ptr, out, len);
                return;
            } else {
                ++ptr;
                memcpy(pos, ptr, flag);
                pos += flag;
                ptr += flag;
                *len += flag;
                if ((int)ptr[0]) {
                    *pos++ = '.';
                    ++(*len);
                }
            }
        }
    }

    void parseResponse(char* buffer, std::vector<DNSQueryResult>& list) {
        char* ptr = buffer;
        ptr += 4;
        int querys = ntohs(*(unsigned short*)ptr);
        ptr += 2;
        int answers = ntohs(*(unsigned short*)ptr);
        ptr += 6;
        for (int i = 0, flag = 0; i < querys; ++i, ptr += 4) {
            do {
                flag = (int)ptr[0];
                ptr += flag + 1;
            } while (flag);
        }
        
        in_addr_t netip;
        int len, type, ttl, datalen;
        for (int i = 0; i < answers; ++i) {
            char aname[128]{};
            len = 0;
            parseName(buffer, ptr, aname, &len);
            ptr += 2;
            type = htons(*(unsigned short*)ptr);
            ptr += 4;
            ttl = htons(*(unsigned short*)ptr);
            ptr += 4;
            datalen = ntohs(*(unsigned short*)ptr);
            ptr += 2;
            if (type == DNS_CNAME) {
                char cname[128]{};
                len = 0;
                parseName(buffer, ptr, cname, &len);
                ptr += datalen;
            } else if (type == DNS_HOST) {
                if (datalen == 4) {
                    netip = *(in_addr_t*)ptr;
                    list.push_back(DNSQueryResult{aname, ::ntohl(netip), ttl});
                }
                ptr += datalen;
            }
        }
    }

}; // class DNSQuery

} // nanonet

#endif // __DNS_QUERY_HPP__