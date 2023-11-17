// utility/log.cpp

#include "nanonet/utility/log.h"

namespace nanonet {

Log::Log(Levels level, std::ostream& out) :level(level), out(out) {}

// print log head like `[time] [level]`
void Log::showHead(Levels level) {
    static const char* const LEVEL_TIPS[4] = {
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
        time_tm->tm_min, time_tm->tm_sec, (int)dis_millseconds, LEVEL_TIPS[(size_t)level]);
    this->out << buffer;
}


// Log object << msg
Log debug(Log::Levels::DEBUG);
Log info(Log::Levels::INFO);
Log warn(Log::Levels::WARN);
Log error(Log::Levels::ERROR);


} // namespace nanonet