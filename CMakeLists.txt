cmake_minimum_required(VERSION 3.11)
project(icplog VERSION 0.1.0 LANGUAGES CXX)

// set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(third_party/fmt)

add_subdirectory(src)
add_subdirectory(tests)