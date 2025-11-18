#pragma once

#include "../common.h"
#include "../level.h"
#include "utils.h"
#include <string>
#include <cstddef>

namespace icplog {
namespace details {
// Source code location information (for debugging)
struct source_loc {
    constexpr source_loc() = default;
    constexpr source_loc(const char* filename, int line, const char* funcname) : filename(filename), line(line), funcname(funcname) {}

    constexpr bool empty() const noexcept { return line == 0; }

    const char* finename{ nullptr};
    int line{0};
    const char* funcname{nullptr};
};

// Log message structure
// This is the core data structure of the logging system, containing all the information for a single log entry
struct log_msg {
    log_msg() = default;

    // constructor: creates the log message
    log_msg(log_clock::time_point log_time,
            source_loc loc,
            string_view_t logger_name,
            icplog::level lvl,
            string_view_t msg)
        : logger_name(logger_name)
        , lvl(lvl)
        , time(log_time)
        , thread_id(get_thread_id())
        , source(loc)
        , payload(msg)
    {}

    // simplified constructor(automatically retrieves the current time)
    log_msg(source_loc loc,
            string_view_t logger_name,
            icplog::level lvl,
            string_view_t msg)
        : log_msg(log_clock::now(), loc, logger_name, lvl, msg)
    {}

    // simplified constructor (no source code location information)
    log_msg(string_view_t logger_name,
            icplog::level lvl,
            string_view_t msg)
        : log_msg(source_loc(), logger_name, lvl, msg)
    {}

    log_msg(const log_msg&) = default;
    log_msg& operator=(const log_msg&) = default;

    // core fields
    string_view_t logger_name;               // Logger name
    //level level{level::off};               // log level
    icplog::level lvl{icplog::level::off};   // use the full path
    log_clock::time_point time;              // timestamp (directly uses standard library types)
    size_t thread_id{0};                     // thread ID
    source_loc source;                       // source code location
    string_view_t payload;                   // actual log content

    // color range (used for formatting, set by the formatter)
    mutable size_t color_range_start{0};
    mutable size_t color_range_end{0};
    
};

} // namespace details
} // namespace icplog