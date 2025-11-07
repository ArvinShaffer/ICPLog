#pragma once

#include "../common.h"
#include <string>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <pthread.h>
#endif

namespace icplog{
namespace details {

// format time as a string
ICPLOG_API std::string format_time(
    const log_clock::time_point& tp,
    const char* format = "%Y-%m-%d %H:%M:%S"
);

// get current timestamp (milliseconds)
ICPLOG_API int64_t get_timestamp_ms();

// get thread id
ICPLOG_API size_t get_thread_id();

// string utilities
ICPLOG_API std::string& ltrim(std::string& s);
ICPLOG_API std::string& rtrim(std::string& s);
ICPLOG_API std::string& trim(std::string& s);

} // namespace details
} // namespace icplog