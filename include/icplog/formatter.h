#pragma once

#include "common.h"
#include "details/log_msg.h"
#include <fmt/format.h>
#include <memory>

namespace icplog {

// the formatter abstract interface
// referencing the spd log design: each sink has one formatter instance
class formatter {
public:
    virtual ~formatter() = default;

    // formatting log messages into a buffer
    // using fmt::memory_buffer for optimal performance
    virtual void format(const details::log_msg& msg, fmt::memory_buffer& dest) = 0;

    // creating a copy of the formatter
    // each sink needs an independent formatter instance to avoid multi-threaded contention
    virtual std::unique_ptr<formatter> clone() const = 0;
};
}  // namespace icplog