cmake_minimum_required(VERSION 3.10.2)

set(zlib-include ${DIR_ROOT_EXTERNAL}/zlib/${ANDROID_ABI}/zlib/include/zlib)
set(zlib-lib ${DIR_ROOT_EXTERNAL}/zlib/${ANDROID_ABI}/zlib/lib)

set(minizip-include ${DIR_ROOT_EXTERNAL}/minizip/${ANDROID_ABI}/minizip/include/minizip)
set(minizip-lib ${DIR_ROOT_EXTERNAL}/minizip/${ANDROID_ABI}/minizip/lib)

set(source apk.cpp)

add_library(apk STATIC ${source})

add_dependencies(apk zlib)
add_dependencies(apk minizip)
add_dependencies(apk frida)

target_link_libraries(apk ${zlib-lib}/libz.a)
target_link_libraries(apk ${zlib-lib}/libminizip.a)
target_link_libraries(apk utils)

target_include_directories(apk PRIVATE ${zlib-include})
target_include_directories(apk PRIVATE ${minizip-include})
target_include_directories(apk PRIVATE utils)
