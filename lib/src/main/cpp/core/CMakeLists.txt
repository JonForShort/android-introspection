cmake_minimum_required(VERSION 3.10.2)

set(source native-lib.cpp)

add_library(native-lib SHARED ${source})

target_link_libraries(native-lib apk)
target_link_libraries(native-lib utils)

target_include_directories(native-lib PRIVATE apk)
target_include_directories(native-lib PRIVATE utils)
