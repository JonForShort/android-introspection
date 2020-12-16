cmake_minimum_required(VERSION 3.10.2)

project(utils CXX)

set(source
        include/utils/log.h
        test.cpp)

add_library(utils STATIC ${source})

target_include_directories(utils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

include("${CMAKE_CURRENT_SOURCE_DIR}/Android.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/LogLevel.cmake")
