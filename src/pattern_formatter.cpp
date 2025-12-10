#include "icplog/pattern_formatter.h"
#include "icplog/details/utils.h"
#include <iomanip>
#include <sstream>
#include <cctype>

namespace icplog {

// ===================================================================
// various flag formatter implementations
// ===================================================================

namespace {

// plain text (non-placeholder)
class raw_string_formatter : public pattern_formatter::flag_formatter {
public:
    explicit raw_string_formatter(std::string str) : str_(std::move(str)) {}

    void format(const details::log_msg&, const std::tm&, fmt::memory_buffer& dest) override {
        dest.append(str_.data(), str_.data() + str_.size());
    }

    std::unique_ptr<flag_formatter> clone() const override {
        return std::make_unique<raw_string_formatter>(str_);
    }

private:
    std::string str_;
};

// %Y - year (4 digits)
class year_formatter : public pattern_formatter::flag_formatter {
public:
    void format(const details::log_msg&, const std::tm& tm_time, fmt::memory_buffer& dest) override {
        fmt::format_to(std::back_inserter(dest), "{:04d}", tm_time.tm_year + 1900);
    }

    std::unique_ptr<flag_formatter> clone() const override {
        return std::make_unique<year_formatter>();
    }
};

// %m - month (01-12)
class month_formatter : public pattern_formatter::flag_formatter {
public:
    void format(const details::log_msg&, const std::tm& tm_time, fmt::memory_buffer& dest) override {
        fmt::format_to(std::back_inserter(dest), "{:02d}", tm_time.tm_mon + 1);
    }

    std::unique_ptr<flag_formatter> clone() const override {
        return std::make_unique<month_formatter>();
    }
};

// %d - date (01-31)
class day_formatter : public pattern_formatter::flag_formatter {
public:
    void format(const details::log_msg&, const std::tm& tm_time, fmt::memory_buffer& dest) override {
        fmt::format_to(std::back_inserter(dest), "{:02d}", tm_time.tm_mday);
    }

    std::unique_ptr<flag_formatter> clone() const override {
        return std::make_unique<day_formatter>();
    }
};

// %H - hour (00-23)
class hour_formatter : public pattern_formatter::flag_formatter {
public:
    void format(const details::log_msg&, const std::tm& tm_time, fmt::memory_buffer& dest) override {
        fmt::format_to(std::back_inserter(dest), "{:02d}", tm_time.tm_hour);
    }
    
    std::unique_ptr<flag_formatter> clone() const override {
        return std::make_unique<hour_formatter>();
    }
};

// %M - minute (00-59)
class minute_formatter : public pattern_formatter::flag_formatter {
public:
    void format(const details::log_msg&, const std::tm& tm_time, fmt::memory_buffer& dest) override {
        fmt::format_to(std::back_inserter(dest), "{:02d}", tm_time.tm_min);
    }
    
    std::unique_ptr<flag_formatter> clone() const override {
        return std::make_unique<minute_formatter>();
    }
};

// %S - second (00-59)
class second_formatter : public pattern_formatter::flag_formatter {
public:
    void format(const details::log_msg&, const std::tm& tm_time, fmt::memory_buffer& dest) override {
        fmt::format_to(std::back_inserter(dest), "{:02d}", tm_time.tm_sec);
    }
    
    std::unique_ptr<flag_formatter> clone() const override {
        return std::make_unique<second_formatter>();
    }
};


// %l - log level (short format, e.g., I/W/E)
class level_formatter : public pattern_formatter::flag_formatter {
public:
    void format(const details::log_msg& msg, const std::tm&, fmt::memory_buffer& dest) override {
        const char* level_str = level_to_short_string(msg.lvl);
        dest.append(level_str, level_str + std::strlen(level_str));
    }
    
    std::unique_ptr<flag_formatter> clone() const override {
        return std::make_unique<level_formatter>();
    }
};

// %L - log level (full format, e.g., info/warn/error)
class level_full_formatter : public pattern_formatter::flag_formatter {
public:
    void format(const details::log_msg& msg, const std::tm&, fmt::memory_buffer& dest) override {
        const char* level_str = level_to_string(msg.lvl);
        dest.append(level_str, level_str + std::strlen(level_str));
    }
    
    std::unique_ptr<flag_formatter> clone() const override {
        return std::make_unique<level_full_formatter>();
    }
};

// %n - Logger name
class name_formatter : public pattern_formatter::flag_formatter {
public:
    void format(const details::log_msg& msg, const std::tm&, fmt::memory_buffer& dest) override {
        dest.append(msg.logger_name.data(), msg.logger_name.data() + msg.logger_name.size());
    }
    
    std::unique_ptr<flag_formatter> clone() const override {
        return std::make_unique<name_formatter>();
    }
};

// %v - actual log content
class payload_formatter : public pattern_formatter::flag_formatter {
public:
    void format(const details::log_msg& msg, const std::tm&, fmt::memory_buffer& dest) override {
        dest.append(msg.payload.data(), msg.payload.data() + msg.payload.size());
    }
    
    std::unique_ptr<flag_formatter> clone() const override {
        return std::make_unique<payload_formatter>();
    }
};

// %t - thread ID
class thread_id_formatter : public pattern_formatter::flag_formatter {
public:
    void format(const details::log_msg& msg, const std::tm&, fmt::memory_buffer& dest) override {
        fmt::format_to(std::back_inserter(dest), "{}", msg.thread_id);
    }
    
    std::unique_ptr<flag_formatter> clone() const override {
        return std::make_unique<thread_id_formatter>();
    }
};
} // anonymous namespace

// ===================================================================
// pattern_formatter implementation
// ===================================================================

pattern_formatter::pattern_formatter(std::string pattern)
    : pattern_(std::move(pattern))
{
    compile_pattern();
}

/*
========== Test 1:Pattern compilation ==========
Pattern: [%Y-%m-%d %H:%M:%S] [%l] %v
Output:  [2025-09-30 03:36:39] [I] Hello, World!

*/
void pattern_formatter::format(const details::log_msg& msg, fmt::memory_buffer& dest) {
    // performance optimization: time caching
    // only re-fetch the tm structure when the number of seconds changes
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(
        msg.time.time_since_epoch()
    );
    
    if (secs != last_log_secs_) {
        cached_tm_ = get_time(msg);
        last_log_secs_ = secs;
    }
    
    // traverse all flag_formatter and complete formatting
    for (auto& formatter : formatters_) {
        formatter->format(msg, cached_tm_, dest);
    }
    
    // add new line character
    dest.push_back('\n');
}

std::unique_ptr<formatter> pattern_formatter::clone() const {
    return std::make_unique<pattern_formatter>(pattern_);
}

void pattern_formatter::set_pattern(std::string pattern) {
    pattern_ = std::move(pattern);
    formatters_.clear();
    compile_pattern();
}

void pattern_formatter::compile_pattern() {
    auto it = pattern_.begin();
    auto end = pattern_.end();
    std::string user_chars;
    
    while (it != end) {
        if (*it == '%') {
            // when encountering %, save the previous plain text first
            if (!user_chars.empty()) {
                formatters_.push_back(
                    std::make_unique<raw_string_formatter>(std::move(user_chars))
                );
                user_chars.clear();
            }
            
            // parse placeholders
            ++it;
            if (it != end) {
                char flag = *it;
                ++it;
                
                // create the corresponding formatter based on the flag
                switch (flag) {
                    case 'Y': formatters_.push_back(std::make_unique<year_formatter>()); break;
                    case 'm': formatters_.push_back(std::make_unique<month_formatter>()); break;
                    case 'd': formatters_.push_back(std::make_unique<day_formatter>()); break;
                    case 'H': formatters_.push_back(std::make_unique<hour_formatter>()); break;
                    case 'M': formatters_.push_back(std::make_unique<minute_formatter>()); break;
                    case 'S': formatters_.push_back(std::make_unique<second_formatter>()); break;
                    case 'l': formatters_.push_back(std::make_unique<level_formatter>()); break;
                    case 'L': formatters_.push_back(std::make_unique<level_full_formatter>()); break;
                    case 'n': formatters_.push_back(std::make_unique<name_formatter>()); break;
                    case 'v': formatters_.push_back(std::make_unique<payload_formatter>()); break;
                    case 't': formatters_.push_back(std::make_unique<thread_id_formatter>()); break;
                    case '%': user_chars += '%'; break;  // %% escaped %
                    default:
                        // unknown placeholder, output as is
                        user_chars += '%';
                        user_chars += flag;
                        break;
                }
            }
        } else {
            // normal characters, accumulated in user_chars
            user_chars += *it;
            ++it;
        }
    }
    
    // process the end of normal text
    if (!user_chars.empty()) {
        formatters_.push_back(
            std::make_unique<raw_string_formatter>(std::move(user_chars))
        );
    }
}

std::tm pattern_formatter::get_time(const details::log_msg& msg) {
    auto time_t_val = log_clock::to_time_t(msg.time);
    std::tm tm_val;
    
#ifdef _WIN32
    localtime_s(&tm_val, &time_t_val);
#else
    localtime_r(&time_t_val, &tm_val);
#endif
    
    return tm_val;
}

} // namespace icplog