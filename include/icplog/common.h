#pragma once 

#include <string>
#include <memory>
#include <cstdint>
#include <chrono>

namespace icplog {

// version information
constexpr const char* VERSION = "0.1.0";

// platform related definitions
#ifdef _WIN32
    #define ICPLOG_WINDOWS
#elif defined(__linux__)
    #define ICPLOG_LINUX
#elif defined(__APPLE__)
    #define ICPLOG_MACOS
#endif


// export symbol definitions
#if defined(_WIN32) && defined(ICPLOG_SHARED)
    #ifdef ICPLOG_BUILD
        #define ICPLOG_API __declspec(dllexport)
    #else
        #define ICPLOG_API __declspec(dllimport)
    #endif
#else
    #define ICPLOG_API
#endif

// portable type alias
using string_view_t = std::string;  // C++11 compatible, can be upgraded to std::string_view later

// clock type definition (referencing spdlog design)
using log_clock = std::chrono::system_clock;

} // namespace icplog