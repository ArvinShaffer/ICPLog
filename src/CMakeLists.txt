set(ICPLOG_SOURCES
    level.cpp 
    formatter.cpp 
    pattern_formatter.cpp 
    details/utils.cpp
)

add_library(icplog STATIC ${ICPLOG_SOURCES})

target_include_directories(icplog PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(icplog PUBLIC fmt::fmt)

target_compile_features(icplog PUBLIC cxx_std_17)