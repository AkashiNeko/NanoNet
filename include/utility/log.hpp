// log.hpp

#pragma once
#ifndef __LOG_HPP__
#define __LOG_HPP__

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

#define OUTPUT_STREAM std::cerr

namespace nanonet {

enum Levels { DEBUG, INFO, WARN, ERROR, CLOSE };

class Logs;

class LogStream {

    // enable display
    bool enabled;

public:

    LogStream(bool enable = true) :enabled(enable) {}

    // logStrean << msg
    template<class T>
    LogStream& operator<<(const T& msg) {
        if (enabled) OUTPUT_STREAM << msg;
        return *this;
    }
    LogStream& operator<<(std::ostream& (*pf)(std::ostream&)) {
        if (enabled) pf(OUTPUT_STREAM);
        return *this;
    }

}; // class LogStream

class Logs {
    // output level
    const Levels level;

    // filter level
    static Levels displayLevel;

private:

    // print log head like `[time] [level]`
    void showLogHead(Levels level) {
        static const char* const levels[4] = {
            COLOR_LIGHT_AMARANTH "[DEBUG]" COLOR_DEFAULT,
            COLOR_LIGHT_GREEN "[INFO]" COLOR_DEFAULT,
            COLOR_LIGHT_YELLOW "[WARN]" COLOR_DEFAULT,
            COLOR_LIGHT_RED "[ERROR]" COLOR_DEFAULT
        };
        // get local date time
        auto now = std::chrono::system_clock::now();
        uint64_t dis_millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
            - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() * 1000;
        time_t tt = std::chrono::system_clock::to_time_t(now);
        auto time_tm = localtime(&tt);
        // print head
        char buffer[64]{};
        sprintf(buffer, COLOR_LIGHT_CYAN "[%d-%02d-%02d %02d:%02d:%02d.%03d] %s ",
            time_tm->tm_year + 1900, time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour,
            time_tm->tm_min, time_tm->tm_sec, (int)dis_millseconds, levels[level]);
        OUTPUT_STREAM << buffer;
    }

public:

    Logs(Levels level) :level(level) {}

    // Logs(type) << msg
    template<class T>
    LogStream operator<<(const T& msg) {
        bool display = (displayLevel <= level);
        if (display) showLogHead(level);
        return LogStream(display) << msg;
    }
    LogStream operator<<(std::ostream& (*pf)(std::ostream&)) {
        bool display = (displayLevel <= level);
        if (display) {
            showLogHead(level);
            pf(OUTPUT_STREAM);
        }
        return LogStream(display);
    }

    // set filter
    inline static void setDisplayLevel(Levels newLevel) { displayLevel = newLevel; }

}; // class Logs

// init displayLevel
Levels Logs::displayLevel = INFO;

namespace Log {

// Log object << msg
Logs debug(DEBUG);
Logs info(INFO);
Logs warn(WARN);
Logs error(ERROR);

} // namespace Logs


} // namespace nanonet

#endif // __LOG_HPP__