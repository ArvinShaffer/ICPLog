#include "icplog/details/log_msg.h"
#include "icplog/sinks/console_sink.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace icplog;

void test_log_msg_creation()
{
    std::cout << "\n================ Test 1: log_msg creation ===============\n";

    // create log message
    details::log_msg msg1("TestLogger", level::info, "Hello, ICPLog!");

    std::cout << "Logger Name: " << msg1.logger_name << "\n";
    std::cout << "Log Level: " << level_to_string(msg1.lvl) << "\n";
    std::cout << "Thread ID: " << msg1.thread_id << "\n";
    std::cout << "Message Content: " << msg1.payload << "\n";
    std::cout << "Source Code location is empty: " << (msg1.source.empty() ? "Yes" : "No") << "\n";
}

void test_source_loc()
{
    std::cout << "\n================= Test 2: Source Code Location Information ====================\n";

    details::source_loc loc("test_sink.cpp", 42, "test_function");
    details::log_msg msg(loc, "TestLogger", level::debug, "Debug message with source info");

    std::cout << "File Name: " << (msg.source.filename ? msg.source.filename : "null") << "\n";
    std::cout << "Line Number: " << msg.source.line << "\n";
    std::cout << "Function Name: " << (msg.source.funcname ? msg.source.funcname : "null") << "\n";
}

void test_console_sink_mt()
{
    std::cout << "\n====================== Test 3: Multi-threaded console sink =================\n";

    // create a thread-safe console sink
    auto sink = std::make_shared<sinks::console_sink_mt>();

    // set log level
    sink->set_level(level::trace);

    // test different log levels
    std::vector<level> levels = {
        level::trace, level::debug, level::info,
        level::warn, level::error, level::critical
    };

    for (auto lvl : levels) {
        std::string msg_content = std::string("This is a ") + level_to_string(lvl) + " message";
        details::log_msg msg("TestLogger", lvl, msg_content);

        if (sink->should_log(lvl)) {
            sink->log(msg);
        }
    }

    sink->flush();
}

void test_level_filtering()
{
    std::cout << "\n================ Test 4: Log Level Filtering ================\n";

    auto sink = std::make_shared<sinks::console_sink_mt>();

    // set level to warn (only output warn and above)
    sink->set_level(level::warn);
    std::cout << "Sink level set to: " << level_to_string(sink->get_level()) << "\n\n";

    std::vector<level> test_levels = {
        level::trace, level::debug, level::info,
        level::warn, level::error, level::critical
    };

    for (auto lvl : test_levels) {
        std::string msg_content = std::string("Testing ") + level_to_string(lvl);
        details::log_msg msg("FilterTest", lvl, msg_content);

        bool should = sink->should_log(lvl);
        std::cout << level_to_string(lvl) << " - " << (should ? "✓ Output will be" : "✗ Filtered") << "\n";

        if (should) {
            sink->log(msg);
        }
    }
}

void test_stderr_sink()
{
    std::cout << "\n=================== Test 5:stderr Sink ===============\n";

    auto err_sink = std::make_shared<sinks::stderr_sink_mt>();
    err_sink->set_level(level::error);

    std::cout << "(The following messages should be output to stderr)\n";

    details::log_msg error_msg("ErrorLogger", level::error, "This is an error message");
    details::log_msg critical_msg("ErrorLogger", level::critical, "This is a critical message");

    err_sink->log(error_msg);
    err_sink->log(critical_msg);
    err_sink->flush();
}

void test_performance_hint()
{
    std::cout << "\n=================== Test 6: Performance comparison tips ===============\n";

    std::cout << " Performance Tips: \n";
    std::cout << "   - console_sink_mt: Multi-threaded safe (using std::mutex) \n";
    std::cout << "   - console_sink_st: Single-threaded version (lock-free, higher performance)\n";
    std::cout << "   - If you are sure you will only use it in a single thread, it is recommended to use the _st version\n";

    auto sink_mt = std::make_shared<sinks::console_sink_mt>();
    auto sink_st = std::make_shared<sinks::console_sink_st>();

    std::cout << "\n Output using the _mt version: \n";
    details::log_msg msg1("MTLogger", level::info, "Thread-safe message");
    sink_mt->log(msg1);

    std::cout << "\n Output using _st version: \n";
    details::log_msg msg2("STLogger", level::info, "Single-threaded message (faster)");
    sink_st->log(msg2);
}

int main()
{
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║   ICPLog Day 2 Testing - Sink System   ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";

    try {
        test_log_msg_creation();
        test_source_loc();
        test_console_sink_mt();
        test_level_filtering();
        test_stderr_sink();
        test_performance_hint();

        std::cout << "\n All tests passed! \n\n";
    } catch (const std::exception& e) {
        std::cerr << "\n Tests failed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}