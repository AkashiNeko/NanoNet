// http_respond.hpp

#pragma once
#ifndef __HTTP_RESPOND_HPP__
#define __HTTP_RESPOND_HPP__

// C
#include <cassert>

// C++
#include <string>
#include <unordered_map>
#include <sstream>

namespace nanonet {

class HTTPRespond {

    std::string version;
    std::string statusCode;
    std::string statusMessage;
    std::string text;
    std::unordered_map<std::string, std::string> headers;

public:

    HTTPRespond() {}

    HTTPRespond(const char* requestText) {
        this->decode(requestText);
    }

    HTTPRespond(const std::string& requestText)
        :HTTPRespond(requestText.c_str()) {}

    void decode(const char* requestText) {
        std::istringstream iss(requestText);
        std::string line;

        std::getline(iss, line);
        parseStatusLine(line);

        while (std::getline(iss, line) && !line.empty()) {
            if (line == "\r") break;
            parseHeader(line);
        }

        std::stringstream ss;
        ss << iss.rdbuf();
        text = ss.str();
    }

    std::string getVersion() const {
        return version;
    }

    std::string getStatusCode() const {
        return statusCode;
    }

    std::string getStatusMessage() const {
        return statusMessage;
    }

    std::string getText() const {
        return text;
    }

    std::string getHeader(const std::string& headerName) const {
        if (headers.count(headerName) > 0) {
            return headers.at(headerName);
        }
        return std::string();
    }

    std::string toString() const {
        std::string result = version + ' ' + statusCode + ' ' + statusMessage + "\r\n";

        for (const auto& [name, value] : headers)
            result += name + ": " + value + "\r\n";

        result += "\r\n" + text;
        return std::move(result);
    }

private:
    void parseStatusLine(const std::string& line) {
        std::istringstream iss(line);
        iss >> version >> statusCode;
        std::getline(iss, statusMessage);
    }

    void parseHeader(const std::string& line) {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 2);
            headers[key] = value;
        }
    }


}; // class HTTPRespond

} // namespace nanonet

#endif // __HTTP_RESPOND_HPP__