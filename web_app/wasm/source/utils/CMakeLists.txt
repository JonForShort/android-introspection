cmake_minimum_required(VERSION 3.10.2)

project(utils CXX)

set(botan-include ${DIR_ROOT_OUT}/external/botan/include/botan-3)
set(botan-lib     ${DIR_ROOT_OUT}/external/botan/lib)

set(source
        include/utils/log.h
        include/utils/utils.h
        include/utils/v8_wrapper.h
        include/utils/data_stream.h
        data_stream.cpp
        sha.cpp
        test.cpp)

add_library(utils STATIC ${source})

target_link_libraries(utils spdlog)
target_link_libraries(utils ${botan-lib}/libbotan-3.a)

target_include_directories(utils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(utils PRIVATE ${botan-include})

include("${CMAKE_CURRENT_SOURCE_DIR}/Android.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/LogLevel.cmake")
