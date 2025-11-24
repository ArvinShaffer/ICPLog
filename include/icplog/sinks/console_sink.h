#pragma once

#include "base_sink.h"
#include <iostream>
#include <mutex>

namespace icplog {
namespace sinks {

// console sink basic template
template<typename ConsoleMutex>
class console_sink : public base_sink<ConsoleMutex> {
public:
    console_sink() = default;
    ~console_sink() override = default;

protected:
    void sink_it_(const details::log_msg& msg) override {
        // simple implementation: direct output to stdout
        // format: [time] [level] message content

        // format time
        std::string time_str = details::format_time(msg.time, "%Y-%m-%d %H:%M:%S");

        // get level string
        const char* level_str = level_to_short_string(msg.lvl);

        // output format: [2025-11-14 16:53:45] [l] Hello World
        std::cout << "[" << time_str << "] "
                  << "[" << level_str << "] "
                  << msg.payload << std::endl;
    }

    void flush_() override {
        std::cout << std::flush;
    }
}; // class console_sink

// multi-threaded version (using null_mutex, higher performance)
using console_sink_mt = console_sink<std::mutex>;

// single-threaded version (using null_mutex, higher performance)
using console_sink_st = console_sink<null_mutex>;

// version with error output to stderr
template<typename ConsoleMutex>
class stderr_sink : public base_sink<ConsoleMutex> {
public:
    stderr_sink() = default;
    ~stderr_sink() override = default;

protected:
    void sink_it_(const details::log_msg& msg) override {
        std::string time_str = details::format_time(msg.time, "%Y-%m-%d %H:%M:%S");
        const char* level_str = level_to_short_string(msg.lvl);

        std::cerr << "[" << time_str << "] "
                  << "[" << level_str << "] "
                  << msg.payload << std::endl;
    }

    void flush_() override {
        std::cerr << std::flush;
    }
};  // class stderr_sink

using stderr_sink_mt = stderr_sink<std::mutex>;
using stderr_sink_st = stderr_sink<null_mutex>;
}   // namespace sinks
}   // namespace icplog