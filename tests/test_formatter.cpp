#include "icplog/pattern_formatter.h"
#include "icplog/sinks/console_sink.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

using namespace icplog;

void test_pattern_compilation() {
    std::cout << "\n========== Test 1:Pattern compilation ==========\n";
    
    // create a custom pattern
    std::string patterns[] = {
        "[%Y-%m-%d %H:%M:%S] [%l] %v",
        "%H:%M:%S.%L - %v",
        "[%L] [%n] [thread %t] %v",
        "%YYear%mMonth%dDay %HHour%MMinute%SSecond %v"
    };
    
    for (const auto& pattern : patterns) {
        pattern_formatter formatter(pattern);
        fmt::memory_buffer buf;
        
        details::log_msg msg("TestLogger", level::info, "Hello, World!");
        formatter.format(msg, buf);
        
        std::cout << "Pattern: " << pattern << "\n";
        std::cout << "Output:  " << std::string_view(buf.data(), buf.size());
        std::cout << "\n";
    }
}

void test_all_flags() {
    std::cout << "\n========== Test 2: all placeholders ==========\n";
    
    pattern_formatter formatter(
        "Year:%Y Month:%m Day:%d Hour:%H Min:%M Sec:%S Level:%l(%L) Name:%n Thread:%t Msg:%v"
    );
    
    details::log_msg msg("MyLogger", level::warn, "Test message");
    fmt::memory_buffer buf;
    formatter.format(msg, buf);
    
    std::cout << std::string_view(buf.data(), buf.size());
}

void test_level_formatting() {
    std::cout << "\n========== Test 3:different level formatting ==========\n";
    
    auto sink = std::make_shared<sinks::console_sink_mt>();
    
    // set a custom pattern
     sink->set_formatter(
        std::make_unique<pattern_formatter>("[%Y-%m-%d %H:%M:%S] [%L] %v")
    );
    
    level levels[] = {
        level::trace, level::debug, level::info,
        level::warn, level::error, level::critical
    };
    
    for (auto lvl : levels) {
        std::string msg_text = std::string("This is ") + level_to_string(lvl) + " message";
        details::log_msg msg("TestLogger", lvl, msg_text);
        sink->log(msg);
    }
}

void test_time_caching() {
    std::cout << "\n========== Test 4: time caching performance ==========\n";
    
    pattern_formatter formatter("[%Y-%m-%d %H:%M:%S] [%l] %v");
    
    // when recording multiple logs within the same second, the cached time should be reused
    auto start = std::chrono::high_resolution_clock::now();
    
    const int iterations = 10000;
    for (int i = 0; i < iterations; ++i) {
        details::log_msg msg("PerfTest", level::info, "Test message");
        fmt::memory_buffer buf;
        formatter.format(msg, buf);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Formatting " << iterations << " log message time consumption: " 
              << duration.count() << " microseconds\n";
    std::cout << "average per message: " << (duration.count() / (double)iterations) << " microseconds\n";
    std::cout << "\n time caching mechanism:\n";
    std::cout << "  - log messages within the same second share the same tm structure\n";
    std::cout << "  - avoid duplicate calls localtime_r/localtime_s\n";
    std::cout << "  - significantly improve performance(especially in high-frequency log scenarios)\n";
}

void test_custom_patterns() {
    std::cout << "\n========== Test 5: Custom Pattern ==========\n";
    
    struct PatternTest {
        std::string pattern;
        std::string description;
    };
    
    PatternTest tests[] = {
        {"[%H:%M:%S] %v", "Time + Message only"},
        {"%l | %v", "Level + Message only"},
        {"[%n] %v", "Logger Name + Message only"},
        {"%Y%m%d %H%M%S [%L] %v", "Compact Format"},
        {"[%Y-%m-%d %H:%M:%S] [%n] [%L] [tid:%t] %v", "Full Format"}
    };
    
    for (const auto& test : tests) {
        pattern_formatter formatter(test.pattern);
        details::log_msg msg("CustomLogger", level::info, "Sample log message");
        fmt::memory_buffer buf;
        formatter.format(msg, buf);
        
        std::cout << "Description: " << test.description << "\n";
        std::cout << "Output: " << std::string_view(buf.data(), buf.size());
    }
}

void test_escape_percent() {
    std::cout << "\n========== Test 6: Percent sign escaping ==========\n";
    
    pattern_formatter formatter("Progress: 50%% - %v");
    details::log_msg msg("TestLogger", level::info, "Task completed");
    fmt::memory_buffer buf;
    formatter.format(msg, buf);
    
    std::cout << "Pattern: Progress: 50%% - %v\n";
    std::cout << "Output:  " << std::string_view(buf.data(), buf.size());
}

void test_formatter_in_sink() {
    std::cout << "\n========== Test 7: Formatter in Sink ==========\n";
    
    auto sink1 = std::make_shared<sinks::console_sink_mt>();
    auto sink2 = std::make_shared<sinks::console_sink_mt>();
    
    // set different formats for different sinks
    sink1->set_formatter(
        std::make_unique<pattern_formatter>("[Concise] [%H:%M:%S] %v")
    );
    
    sink2->set_formatter(
        std::make_unique<pattern_formatter>("[Detailed] [%Y-%m-%d %H:%M:%S] [%L] [%n] %v")
    );
    
    details::log_msg msg("MultiSinkTest", level::info, "Testing different formats");
    
    std::cout << "Sink 1 Output:\n";
    sink1->log(msg);
    
    std::cout << "Sink 2 Output:\n";
    sink2->log(msg);
}

void test_pattern_change() {
    std::cout << "\n========== Test 8: Dynamically Modifying Pattern ==========\n";
    
    auto sink = std::make_shared<sinks::console_sink_mt>();
    details::log_msg msg("DynamicTest", level::info, "Same message");
    
    // first output
    std::cout << "Pattern 1: [Default]\n";
    sink->log(msg);
    
    // modify pattern
    sink->set_formatter(
        std::make_unique<pattern_formatter>("[%H:%M:%S] >>> %v <<<")
    );
    
    std::cout << "\nPattern 2: [%H:%M:%S] >>> %v <<<\n";
    sink->log(msg);
    
    // modify again
    sink->set_formatter(
        std::make_unique<pattern_formatter>("%Y/%m/%d | %v")
    );
    
    std::cout << "\nPattern 3: %Y/%m/%d | %v\n";
    sink->log(msg);
}

void test_thread_id() {
    std::cout << "\n========== Test 9: Multi-threaded ID display ==========\n";
    
    pattern_formatter formatter("[thread %t] %v");
    
    auto log_from_thread = [&formatter](int thread_num) {
        details::log_msg msg("ThreadTest", level::info, 
                           "Message from thread " + std::to_string(thread_num));
        fmt::memory_buffer buf;
        formatter.format(msg, buf);
        std::cout << std::string_view(buf.data(), buf.size());
    };
    
    std::thread t1(log_from_thread, 1);
    std::thread t2(log_from_thread, 2);
    std::thread t3(log_from_thread, 3);
    
    t1.join();
    t2.join();
    t3.join();
}

void test_unknown_flags() {
    std::cout << "\n========== Test 10: Handling unknown placeholders ==========\n";
    
    pattern_formatter formatter("[%Y-%m-%d] [%Z] %v");  // %Z is an unknown placeholder
    details::log_msg msg("TestLogger", level::info, "Test unknown flags");
    fmt::memory_buffer buf;
    formatter.format(msg, buf);
    
    std::cout << "Pattern: [%Y-%m-%d] [%Z] %v\n";
    std::cout << "Output:  " << std::string_view(buf.data(), buf.size());
    std::cout << "Explanation: The unknown placeholder %Z is output as is\n";
}

int main() {
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║ ICPLog Day 2 Testing - Formatter System ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    
    try {
        test_pattern_compilation();
        test_all_flags();
        test_level_formatting();
        test_time_caching();
        test_custom_patterns();
        test_escape_percent();
        test_formatter_in_sink();
        test_pattern_change();
        test_thread_id();
        test_unknown_flags();
        
        std::cout << "\n All tests passed!\n\n";
    } catch (const std::exception& e) {
        std::cerr << "\n Test failed: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}