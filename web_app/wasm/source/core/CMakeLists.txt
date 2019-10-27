cmake_minimum_required(VERSION 3.10.2)

set(source core.cpp)

add_library(core STATIC ${source})

target_link_libraries(core apk)
target_link_libraries(core utils)

target_include_directories(core PRIVATE apk)
target_include_directories(core PRIVATE utils)
