# Test 01: Log level test
add_executable(test_level test_level.cpp)
target_link_libraries(test_level PRIVATE icplog)

# Test 02: Sink and Log Message test
add_executable(test_sink test_sink.cpp)
target_link_libraries(test_sink PRIVATE icplog)
