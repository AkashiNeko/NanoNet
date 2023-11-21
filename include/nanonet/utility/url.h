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

    void parseUserInfo(std::string&& userInfo) {
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

    void parseAuthority(std::string&& authority) {
        // http://user:password@example.com:8080/abc/def?a=1&b=2#section
        //        \____________________________/
        //                      |
        //                  authority

        size_t posAt = authority.find_first_of('@');
        size_t pos = 0;
        if (posAt != std::string::npos) {
            pos = posAt + 1;
            parseUserInfo(authority.substr(0, posAt));
        }
        // parse host port
        size_t posPort = authority.find_first_of(':', pos);
        if (posPort != std::string::npos) {
            this->host = authority.substr(pos, posPort - pos);
            pos = posPort + 1;
            std::string strPort = authority.substr(posPort + 1);
            if (strPort.empty()) {
                
            }
            Port p;
            try {
                p = Port::parsePort(strPort);
            } catch (const ParsePortError&) {
                throwError<ParseUrlError>("[url] port \'", strPort, "\' is invalid");
            }
            this->port = p;
        } else {
            this->host = authority.substr(pos);
        }
    }

    void parseAuthorityAfter(std::string&& str) {
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

    void parseUrl(const std::string& str) {
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

        parseAuthority(str.substr(pos, authorityEnd - pos));
        if (authorityEnd != std::string::npos) {
            parseAuthorityAfter(str.substr(authorityEnd));
        }
    }

public:

    Url(const std::string& url) {
        parseUrl(url);
    }

    inline static Port getDefaultPort(const std::string& scheme) {
        static std::unordered_map<std::string, Port> defaultPortMap = {
            {"http", 80},
            {"https", 443},
            {"ftp", 21},
            {"telnet", 23},
            {"ssh", 22},
            {"smtp", 25},
            {"imap", 143},
            {"pop3", 110},
            {"dns", 53},
            {"sftp", 22},
            {"ldap", 389},
            {"rdp", 3389},
            {"mysql", 3306},
            {"postgresql", 5432},
            {"mongodb", 27017},
            {"redis", 6379},
            {"mqtt", 1883},
            {"ntp", 123},
            {"rtmp", 1935},
            {"vnc", 5900},
            {"irc", 6667}
        };
        auto it = defaultPortMap.find(scheme);
        if (it == defaultPortMap.end())
            return 0;
        else
            return it->second;
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

    std::string test() const {
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