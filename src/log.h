// log.h

#pragma once
#ifndef __LOG_H__
#define __LOG_H__

// C
#include <cstdint>
#include <cstring>

// C++
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

// linux
#include <netinet/in.h>

#define COLOR_DEFAULT    "\033[0m"

#define COLOR_BLACK      "\033[30m"
#define COLOR_RED        "\033[31m"
#define COLOR_GREEN      "\033[32m"
#define COLOR_YELLOW     "\033[33m"
#define COLOR_BLUE       "\033[34m"
#define COLOR_AMARANTH   "\033[35m"
#define COLOR_CYAN       "\033[36m"
#define COLOR_WHITE      "\033[37m"

#define COLOR_LIGHT            "\033[1m"
#define COLOR_LIGHT_BLACK      "\033[30;1m"
#define COLOR_LIGHT_RED        "\033[31;1m"
#define COLOR_LIGHT_GREEN      "\033[32;1m"
#define COLOR_LIGHT_YELLOW     "\033[33;1m"
#define COLOR_LIGHT_BLUE       "\033[34;1m"
#define COLOR_LIGHT_AMARANTH   "\033[35;1m"
#define COLOR_LIGHT_CYAN       "\033[36;1m"
#define COLOR_LIGHT_WHITE      "\033[37;1m"

namespace nano {

class Log {
public:
    enum class Levels { DEBUG, INFO, WARN, ERROR, CLOSE };

private:
    // output levels
    const Levels level;
    std::ostream& out;

public:

    Log(Levels level, std::ostream& out = std::cerr);

    // print log head like `[time] [level]`
    void showHead(Levels level);

    // Logs(type) << msg
    template<class T>
    std::ostream& operator<<(const T& msg) {
        showHead(level);
        return this->out << msg;
    }
    std::ostream& operator<<(std::ostream& (*pf)(std::ostream&)) {
        showHead(level);
        pf(this->out);
        return std::cerr;
    }

}; // class Logs

// Log object << msg
extern Log debug;
extern Log info;
extern Log warn;
extern Log error;


} // namespace nano

#endif // __LOG_H__