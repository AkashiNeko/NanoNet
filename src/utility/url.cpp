// utility/url.cpp

#include "nanonet/utility/url.h"

namespace nanonet {

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

static void _encodeString(std::string& result, std::string&& str, const char* ignore = "") {
    for (char c : str) {
        if (c == ' ') {
            result += '+';
        } else if (_inIgnore(c, ignore) || isalnum(c) || c == '%'
            || c == '.' || c == '_' || c == '*' || c == '-') {
            result += c;
        } else {
            result += '%';
            result += _toHex(c >> 4);
            result += _toHex(c);
        }
    }
}

// parse

void Url::_parseUserInfo(std::string&& userInfo) {
    // http://user:password@example.com:8080/abc/def?a=1&b=2#section
    //        \___________/
    //              |
    //           userinfo

    size_t pos = userInfo.find_first_of(':');
    if (pos == std::string::npos) {
        _encodeString(this->user, std::move(userInfo));
    } else {
        _encodeString(this->user, userInfo.substr(0, pos));
        _encodeString(this->password, userInfo.substr(pos + 1));
    }
}

void Url::_parseAuthority(std::string&& authority) {
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
        _encodeString(this->host, authority.substr(pos, posPort - pos), "/");
        pos = posPort + 1;
        std::string strPort = authority.substr(posPort + 1);
        if (strPort.empty()) {
            this->port = _getDefaultPort(this->scheme);
        } else {
            try {
                this->port.parse(strPort);
            } catch (const ParsePortError&) {
                throwError<ParseUrlError>("[url] port \'", strPort, "\' is invalid");
            }
        }
    } else {
        this->port = _getDefaultPort(this->scheme);
        this->host = authority.substr(pos);
    }
}

void Url::_parseAuthorityAfter(std::string&& str) {
    // /abc/def?a=1&b=2#section
    // \______________________/
    //            |
    //     authority after

    if (str.empty()) return;

    // /abc/def?a=1&b=2#section
    // \______/
    //    |
    //   path
    size_t queryPos = str.find_first_of('?');
    size_t fragmentPos = str.find_first_of('#');
    size_t pathEnd = std::min(queryPos, fragmentPos);
    if (str.front() == '/') {
        this->path.clear();
        _encodeString(this->path, str.substr(0, pathEnd), "/");
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
        _encodeString(this->fragment, str.substr(pathEnd + 1));
    } else if (fragmentPos == std::string::npos) {
        // no fragment
        _encodeString(this->query, str.substr(pathEnd + 1), "=&?");
    } else {
        if (fragmentPos < queryPos) {
            // #....?....
            _encodeString(this->query, str.substr(queryPos + 1), "=&?");
            _encodeString(this->fragment, str.substr(fragmentPos + 1, queryPos - fragmentPos - 1));
        } else {
            // ?....#....
            _encodeString(this->query, str.substr(queryPos + 1, fragmentPos - queryPos - 1), "=&?");
            _encodeString(this->fragment, str.substr(fragmentPos + 1));
        }
    }
}

void Url::_parseUrl() {
    // http://user:password@example.com:8080/abc/def?a=1&b=2#section
    // \__/
    //   |
    // scheme
    size_t pos = raw.find_first_of(":");
    if (pos == std::string::npos || pos + 2 >= raw.size()
        || raw.substr(pos + 1, 2) != "//") {
        throwError<ParseUrlError>("[url] invalid url: \'", raw, '\'');
    }
    this->scheme = raw.substr(0, pos);

    // get authority and what follows
    pos += 3;
    size_t authorityEnd = std::string::npos;
    for (int i = pos; i < raw.size(); ++i) {
        if (raw[i] == '/' || raw[i] == '?' || raw[i] == '#') {
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

    _parseAuthority(raw.substr(pos, authorityEnd - pos));
    if (authorityEnd != std::string::npos) {
        _parseAuthorityAfter(raw.substr(authorityEnd));
    }
}


Url::Url(std::string&& url) :raw(std::move(url)) {
    _parseUrl();
}

Url::Url(const std::string& url) :raw(url) {
    _parseUrl();
}

Url::Url(Url&& url)
    : raw(std::move(url.raw)), scheme(std::move(url.scheme)),
    user(std::move(url.user)), password(std::move(url.password)),
    host(std::move(url.host)), path(std::move(url.path)),
    query(std::move(url.query)), fragment(std::move(url.fragment)),
    port(url.port) {
    url.port = 0;
}

void Url::parse(const std::string& url) {
    this->raw = url;
    _parseUrl();
}

void Url::parse(std::string&& url) {
    this->raw = std::move(url);
    _parseUrl();
}

std::string Url::getAuthorityAfter() const {
    std::string result;
    result += this->path;
    if (!this->query.empty())
        result += '?' + this->query;
    if (!this->fragment.empty())
        result += '#' + this->fragment;
    return result;
}

void Url::setAuthorityAfter(const std::string& authority) {
    _parseAuthorityAfter(std::string(authority));
}

void Url::setAuthorityAfter(std::string&& authority) {
    _parseAuthorityAfter(std::move(authority));
}

std::string Url::toString() const {
    if (this->scheme.empty()) return "";
    std::string result = this->scheme + "://";
    if (!this->user.empty()) {
        if (!this->password.empty()) {
            result += this->user + ':' + this->password + '@';
        } else {
            result += this->user + '@';
        }
    }
    result += this->host;
    if (this->port != 0 && this->port != _getDefaultPort(this->scheme)) {
        result += ":" + this->port.toString();
    }
    result += this->getAuthorityAfter();
    return result;
}

}; // namespace nanonet