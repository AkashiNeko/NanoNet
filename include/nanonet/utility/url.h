// url.h

#pragma once
#ifndef __URL_H__
#define __URL_H__

#include <string>
#include <unordered_map>

#include "nanonet/exception/nanoerr.h"
#include "nanonet/utility/addr.h"
#include "nanonet/utility/port.h"

namespace nanonet {

// +-----------------------------------------------------------+
// | reference:  https://datatracker.ietf.org/doc/html/rfc3986 |
// +-----------------------------------------------------------+
// |    foo://example.com:8042/over/there?name=ferret#nose     |
// |    \_/   \______________/\_________/ \_________/ \__/     |
// |     |           |            |            |        |      |
// |  scheme     authority       path        query  fragment   |
// |     |   _____________________|__                          |
// |    / \ /                        \                         |
// |    urn:example:animal:ferret:nose                         |
// +-----------------------------------------------------------+

class Url {

    // url string
    std::string strUrl;

    // scheme
    std::string scheme;
    
    // authority

    // userinfo
    std::string user;
    std::string password;

    // host and port
    std::string host;
    Port port;

    // path
    std::string path;

    // query
    std::string query;

    // fragment
    std::string fragment;

    // example:
    // http://user:password@example.com:8080/abc/def?a=1&b=2#section
    // \__/   \__/ \______/ \_________/ \__/\______/ \_____/ \_____/
    //  |       \     /          |        |    |        |       |
    // scheme   userinfo       host    port  path    query  fragment
    //              \____________|______/
    //                        |
    //                    authority

    inline static Port _getDefaultPort(const std::string& scheme) {
        const static std::unordered_map<std::string, Port> defaultPortMap = {
            { "ftp", 21 }, { "ssh", 22 }, { "sftp", 22 }, { "telnet", 23 },
            { "smtp", 25 }, { "dns", 53 }, { "gopher", 70 }, { "http", 80 },
            { "pop3", 110 }, { "nntp", 119 }, { "ntp", 123 }, { "imap", 143 },
            { "snmp", 161 }, { "snmp-trap", 162 }, { "ldap", 389 }, { "https", 443 },
            { "smb", 445 }, { "smtps", 465 }, { "afp", 548 }, { "ldapssl", 636 },
            { "ldaps", 636 }, { "rsync", 873 }, { "imaps", 993 }, { "pop3s", 995 },
            { "mssql", 1433 }, { "oracle", 1521 }, { "mqtt", 1883 },
            { "rtmp", 1935 }, { "nfs", 2049 }, { "zookeeper", 2181 }, { "etcd", 2379 },
            { "mysql", 3306 }, { "rdp", 3389 }, { "svn", 3690 }, { "sybase", 5000 },
            { "xmpp", 5222 }, { "xmpps", 5223 }, { "postgresql", 5432 },
            { "rabbitmq", 5672 }, { "vnc", 5900 }, { "couchdb", 5984 }, { "redis", 6379 },
            { "irc", 6667 }, { "redis-cluster", 7000 }, { "neo4j", 7687 },
            { "consul", 8500 }, { "hadoop", 9000 }, { "cassandra", 9042 },
            { "kafka", 9092 }, { "elasticsearch", 9200 }, { "git", 9418 },
            { "hive", 10000 }, { "memcached", 11211 }, { "minecraft", 25565 },
            { "redis-sentinel", 26379 }, { "mongodb", 27017 }, { "db2", 50000 },
        };
        auto it = defaultPortMap.find(scheme);
        if (it == defaultPortMap.end())
            return 0;
        else
            return it->second;
    }

    void _parseUserInfo(std::string&& userInfo) {
        // http://user:password@example.com:8080/abc/def?a=1&b=2#section
        //        \___________/
        //              |
        //           userinfo

        size_t pos = userInfo.find_first_of(':');
        if (pos == std::string::npos) {
            this->user = std::move(userInfo);
        } else {
            this->user = userInfo.substr(0, pos);
            this->password = userInfo.substr(pos + 1);
        }
    }

    void _parseAuthority(std::string&& authority) {
        // http://user:password@example.com:8080/abc/def?a=1&b=2#section
        //        \____________________________/
        //                      |
        //                  authority

        size_t posAt = authority.find_first_of('@');
        size_t pos = 0;
        if (posAt != std::string::npos) {
            pos = posAt + 1;
            _parseUserInfo(authority.substr(0, posAt));
        }
        // parse host port
        size_t posPort = authority.find_first_of(':', pos);
        if (posPort != std::string::npos) {
            this->host = authority.substr(pos, posPort - pos);
            pos = posPort + 1;
            std::string strPort = authority.substr(posPort + 1);
            if (strPort.empty()) {
                this->port = _getDefaultPort(this->scheme);
            } else {
                Port p;
                try {
                    p = Port::parsePort(strPort);
                } catch (const ParsePortError&) {
                    throwError<ParseUrlError>("[url] port \'", strPort, "\' is invalid");
                }
                this->port = p;
            }
        } else {
            this->host = authority.substr(pos);
        }
    }

    void _parseAuthorityAfter(std::string&& str) {
        // /abc/def?a=1&b=2#section
        // \______________________/
        //            |
        //     authority after

        if (str.empty()) {
            this->path = "/";
            return;
        }

        // /abc/def?a=1&b=2#section
        // \______/
        //    |
        //   path
        size_t queryPos = str.find_first_of('?');
        size_t fragmentPos = str.find_first_of('#');
        size_t pathEnd = std::min(queryPos, fragmentPos);
        if (str.front() == '/') {
            this->path = str.substr(0, pathEnd);
        }

        // /abc/def?a=1&b=2#section
        //          \_____/ \_____/
        //             |       |
        //           query  fragment
        if (queryPos == std::string::npos && fragmentPos == std::string::npos) {
            // no query no fragment
            return;
        } else if (queryPos == std::string::npos) {
            // no query
            this->fragment = str.substr(pathEnd + 1);
        } else if (fragmentPos == std::string::npos) {
            // no fragment
            this->query = str.substr(pathEnd + 1);
        } else {
            if (fragmentPos < queryPos) {
                // #....?....
                this->query = str.substr(queryPos + 1);
                this->fragment = str.substr(fragmentPos + 1, queryPos - fragmentPos - 1);
            } else {
                // ?....#....
                this->query = str.substr(queryPos + 1, fragmentPos - queryPos - 1);
                this->fragment = str.substr(fragmentPos + 1);
            }
        }
    }

    void _parseUrl(const std::string& str) {
        // http://user:password@example.com:8080/abc/def?a=1&b=2#section
        // \__/
        //   |
        // scheme
        size_t pos = str.find_first_of(":");
        if (pos == std::string::npos || pos + 2 >= str.size()
            || str.substr(pos + 1, 2) != "//") {
            throwError<ParseUrlError>("[url] invalid url: \'", str, '\'');
        }
        this->scheme = str.substr(0, pos);

        // get authority and what follows
        pos += 3;
        size_t authorityEnd = std::string::npos;
        for (int i = pos; i < str.size(); ++i) {
            if (str[i] == '/' || str[i] == '?' || str[i] == '#') {
                authorityEnd = i; break;
            }
        }

        // http://user:password@example.com:8080/abc/def?a=1&b=2#section
        //        A                             A                      A
        //        |                             |                      |
        //       pos                       authorityEnd               end
        //        \____________  ______________/\__________  __________/
        //                     \/                          \/
        //                parseAuthority           parseAuthorityAfter

        _parseAuthority(str.substr(pos, authorityEnd - pos));
        if (authorityEnd != std::string::npos) {
            _parseAuthorityAfter(str.substr(authorityEnd));
        }
    }

    inline static char _toHex(const char c) {
        int mask = c & 0xF;
        return (mask > 9 ? 'A' - 10 : '0') + mask;
    }

    inline static bool _inIgnore(const char c, const char* ignore) {
        if (c == '\0') return false;
        for (const char* p = ignore; *p; ++p)
            if (c == *p) return true;
        return false;
    }

    static void _encodeString(std::string& str, const char* ignore = "") {
        std::string result;
        for (char c : str) {
            if (c == ' ') {
                result += '+';
            } else if (_inIgnore(c, ignore) || isalnum(c)
                || c == '.' || c == '_' || c == '*' || c == '-') {
                result += c;
            } else {
                result += '%';
                result += _toHex(c >> 4);
                result += _toHex(c);
            }
        }
        str = std::move(result);
    }

public:

    Url(const std::string& url) {
        _parseUrl(url);
    }

    static Url parse(const std::string& url) {
        Url urlObj(url);
        _encodeString(urlObj.fragment);
        _encodeString(urlObj.query, "=&?");
        _encodeString(urlObj.path, "/");
        return std::move(urlObj);
    }

    inline std::string getScheme() const {
        return this->scheme;
    }

    inline std::string getUser() const {
        return this->user;
    }

    inline std::string getPassword() const {
        return this->password;
    }

    inline std::string getHost() const {
        return this->host;
    }

    inline Port getPort() const {
        return this->port;
    }

    inline std::string getPath() const {
        return this->path;
    }

    inline std::string getQuery() const {
        return this->query;
    }

    inline std::string getFragment() const {
        return this->fragment;
    }

    std::string toString() const {
        std::string result = getScheme() + "://";
        if (!user.empty()) {
            if (!password.empty()) {
                result += user + ':' + password + '@';
            } else {
                result += user + '@';
            }
        }
        result += host;
        if (port != 0) {
            result += ":" + port.toString();
        }
        result += path;
        if (!query.empty())
            result += '?' + query;
        if (!fragment.empty())
            result += '#' + fragment;
        return result;
    }

}; // class Url

} // namespace nanonet

#endif // __URL_H__