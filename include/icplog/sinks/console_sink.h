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
        fmt::memory_buffer formatted;
        this->format_message(msg, formatted);

        // output to stdout
        std::cout.write(formatted.data(), formatted.size());
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
        fmt::memory_buffer formatted;
        this->format_message(msg, formatted);

        std::cerr.write(formatted.data(), formatted.size());
    }

    void flush_() override {
        std::cerr << std::flush;
    }
};  // class stderr_sink

using stderr_sink_mt = stderr_sink<std::mutex>;
using stderr_sink_st = stderr_sink<null_mutex>;
}   // namespace sinks
}   // namespace icplog