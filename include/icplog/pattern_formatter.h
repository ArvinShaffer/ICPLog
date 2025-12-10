#pragma once 

#include "formatter.h"
#include "level.h"
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <ctime>

namespace icplog {

// pattern_formatter: a formatter base on a pattern string
// supports placeholder syntax similar to strftime
class pattern_formatter : public formatter {
public:
    // constructor: accepts a pattern string
    // pattern example: "[%Y-%m-%d %H:%M:%S] [%l] [%n] %v"
    explicit pattern_formatter(
        std::string pattern = "[%Y-%m-%d %H:%M:%S] [%l] %v"
    );

    ~pattern_formatter() override = default;

    // implement the formatter interface
    void format(const details::log_msg& msg, fmt::memory_buffer& dest) override;
    std::unique_ptr<formatter> clone() const override;

    // set a new pattern (recompile)
    void set_pattern(std::string pattern);

public: 
    // flag_formatter abstract base class : handles single placeholders
    class flag_formatter {
    public:
        virtual ~flag_formatter() = default;
        virtual void format(const details::log_msg& msg,
                            const std::tm& tm_time,
                            fmt::memory_buffer& dest) = 0;
        virtual std::unique_ptr<flag_formatter> clone() const = 0;
    };
private:
    // compiles the pattern string into a flag_formatter vector
    void compile_pattern();

    // get the formatted time structure
    std::tm get_time(const details::log_msg& msg);

    std::string pattern_;                                       // pattern string
    std::vector<std::unique_ptr<flag_formatter>> formatters_;   // flag_formatter vector

    // performance optimization: time caching
    std::chrono::seconds last_log_secs_{0};          // seconds since the last log entry
    std::tm cached_tm_{};                            // cached tm structure
};
} // namespace icplog