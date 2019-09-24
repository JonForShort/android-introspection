cmake_minimum_required (VERSION 3.10.2)

set (zlib-include ${DIR_ROOT_EXTERNAL}/zlib/${ANDROID_ABI}/include/zlib)
set (zlib-lib     ${DIR_ROOT_EXTERNAL}/zlib/${ANDROID_ABI}/lib)

set (source apk.cpp)

add_library (apk STATIC ${source})

add_dependencies (apk zlib)

target_link_libraries (apk ${zlib-lib}/libz.a)
target_link_libraries (apk utils)

target_include_directories (native-lib PRIVATE ${zlib-include})
target_include_directories (apk PRIVATE utils)


