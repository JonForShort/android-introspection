cmake_minimum_required(VERSION 3.10.2)

set(source apk.cpp)

add_library(apk STATIC ${source})

target_link_libraries(apk zlibstatic)
target_link_libraries(apk minizip)
target_link_libraries(apk utils)

target_include_directories(apk PRIVATE zlibstatic)
target_include_directories(apk PRIVATE minizip)
target_include_directories(apk PRIVATE utils)

target_include_directories(apk PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(apk PRIVATE ${DIR_ROOT_EXTERNAL}/zlib/source)
target_include_directories(apk PRIVATE ${DIR_ROOT_EXTERNAL}/minizip/source)
