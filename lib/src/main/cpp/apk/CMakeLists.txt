cmake_minimum_required(VERSION 3.10.2)

set(zlib-out ${DIR_ROOT_OUT}/external/zlib/${ANDROID_ABI}/zlib)
set(zlib-include ${zlib-out}/include/zlib)
set(zlib-lib ${zlib-out}/lib)

set(minizip-out ${DIR_ROOT_OUT}/external/zlib/${ANDROID_ABI}/minizip)
set(minizip-include ${minizip-out}/include/minizip)
set(minizip-lib ${minizip-out}/lib)

set(source apk.cpp)

add_library(apk STATIC ${source})

target_link_libraries(apk ${zlib-lib}/libz.a)
target_link_libraries(apk ${minizip-lib}/libminizip.a)
target_link_libraries(apk utils)

target_include_directories(apk PRIVATE ${zlib-include})
target_include_directories(apk PRIVATE ${minizip-include})
target_include_directories(apk PRIVATE utils)
