#pragma once

#include "common.h"
#include <string>

namespace icplog {

// log level enumeration
enum class level {
    trace = 0,     // most detailed debugging information
    debug = 1,     // debugging information
    info = 2,      // general information
    warn = 3,      // warning information
    error = 4,     // error information
    critical = 5,  // serious error
    off = 6        // disable logging
};

// convert level to string
ICPLOG_API const char* level_to_string(level lvl) noexcept;

// convert level to short string (for formatting)
ICPLOG_API const char* level_to_short_string(level lvl) noexcept;

// convert string to level
ICPLOG_API level string_to_level(const std::string& str);

// level comparison function
inline bool should_log(level logger_level, level msg_level) noexcept {
    return msg_level >= logger_level;
}
}  // namespace icplog