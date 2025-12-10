#pragma once

#include "../common.h"
#include "../details/log_msg.h"
#include "../formatter.h"
#include "../pattern_formatter.h"
#include <mutex>
#include <memory>

namespace icplog {
namespace sinks {

// sink interface class (pure virtual)
class sink {
public: 
    virtual ~sink() = default;

    // output log (thread-safe)
    virtual void log(const details::log_msg& msg) = 0;

    // flush buffer
    virtual void flush() = 0;

    // set log level
    virtual void set_level(level log_level) = 0;
    virtual level get_level() const = 0;

    // determine whether to output
    virtual bool should_log(level msg_level) const = 0;

    // formatter interface
    virtual void set_formatter(std::unique_ptr<formatter> sink_formatter) = 0;
}; // class sink

// base_sink: implements a thread-safe sink base class
// template parameters mutex can be std::mutex or null_mutex (single-thread version)
template<typename Mutex>
class base_sink : public sink {
public:
    base_sink() : level_(level::trace), formatter_(std::make_unique<pattern_formatter>()) {}

    base_sink(const base_sink&) = delete;
    base_sink& operator=(const base_sink&) = delete;

    void log(const details::log_msg& msg) override {
        std::lock_guard<Mutex> lock(mutex_);
        sink_it_(msg);
    }

    void flush() override {
        std::lock_guard<Mutex> lock(mutex_);
        flush_();
    }

    void set_level(level log_level) override {
        std::lock_guard<Mutex> lock(mutex_);
        level_ = log_level;
    }

    level get_level() const override {
        std::lock_guard<Mutex> lock(mutex_);
        return level_;
    }

    bool should_log(level msg_level) const override {
        return msg_level >= level_;
    }

    void set_formatter(std::unique_ptr<formatter> sink_formatter) override {
        std::lock_guard<Mutex> lock(mutex_);
        formatter_ = std::move(sink_formatter);
    }

protected:
    // core methods that subclasses need to implement (locked, no need to worry about thread safety)
    virtual void sink_it_(const details::log_msg& msg) = 0;
    virtual void flush_() = 0;

    // formatting log messages
    void format_message(const details::log_msg& msg, fmt::memory_buffer& dest) {
        formatter_->format(msg, dest);
    }

    mutable Mutex mutex_; // mutex lock
    level level_;         // log level
    std::unique_ptr<formatter> formatter_;   // each sink has its own formatter
}; // base_sink

// null_mutex: used for the single-threaded version of sink(lock-free, higher performance)
struct null_mutex {
    void lock() {}
    void unlock() {}
};
} // namespace sinks
} // namespace icplog