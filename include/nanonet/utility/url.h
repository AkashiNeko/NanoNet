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
    std::string raw;

    // scheme
    std::string scheme;
    
    // userinfo
    std::string user;
    std::string password;

    // host
    std::string host;

    // path
    std::string path = "/";

    // query
    std::string query;

    // fragment
    std::string fragment;

    // port
    Port port = 0;

private:

    void _parseUserInfo(std::string&& userInfo);
    void _parseAuthority(std::string&& authority);
    void _parseAuthorityAfter(std::string&& str);
    void _parseUrl();

public:

    Url() = default;
    Url(std::string&& url);
    Url(const std::string& url);
    Url(Url&& url);

    inline Url(const char* url) :Url(std::string(url)) {}

    virtual ~Url() = default;

    void parse(std::string&& url);
    void parse(const std::string& url);

    std::string getAuthorityAfter() const;
    void setAuthorityAfter(const std::string& authority);
    void setAuthorityAfter(std::string&& authority);

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

    inline void setScheme(const std::string& scheme) {
        this->scheme = scheme;
    }
    inline void setScheme(std::string&& scheme) {
        this->scheme = std::move(scheme);
    }

    inline void setUser(const std::string& user) {
        this->user = user;
    }
    inline void setUser(std::string&& user) {
        this->user = std::move(user);
    }

    inline void setPassword(const std::string& password) {
        this->password = password;
    }
    inline void setPassword(std::string&& password) {
        this->password = std::move(password);
    }

    inline void setHost(const std::string& host) {
        this->host = host;
    }
    inline void setHost(std::string&& host) {
        this->host = std::move(host);
    }

    inline void setPort(Port port) {
        this->port = port;
    }

    inline void setPath(const std::string& path) {
        this->path = path;
    }
    inline void setPath(std::string&& path) {
        this->path = std::move(path);
    }

    inline void setQuery(const std::string& query) {
        this->query = query;
    }
    inline void setQuery(std::string&& query) {
        this->query = std::move(query);
    }

    inline void setFragment(const std::string& fragment) {
        this->fragment = fragment;
    }
    inline void setFragment(std::string&& fragment) {
        this->fragment = std::move(fragment);
    }

    // to string
    std::string toString() const;

}; // class Url

} // namespace nanonet

#endif // __URL_H__