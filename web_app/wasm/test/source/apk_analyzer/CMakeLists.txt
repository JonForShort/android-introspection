cmake_minimum_required(VERSION 3.10.2)

set(source apk_analyzer.cpp)

add_library(test_apk_analyzer STATIC ${source})

target_include_directories(test_apk_analyzer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
