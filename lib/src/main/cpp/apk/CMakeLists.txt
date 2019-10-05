cmake_minimum_required(VERSION 3.10.2)

set(source apk.cpp)

add_library(apk STATIC ${source})

target_link_libraries(apk zlib)
target_link_libraries(apk minizip)
target_link_libraries(apk utils)

target_include_directories(apk PRIVATE zlib)
target_include_directories(apk PRIVATE minizip)
target_include_directories(apk PRIVATE utils)
