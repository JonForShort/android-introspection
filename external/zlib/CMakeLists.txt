cmake_minimum_required(VERSION 3.10.2)

include(ExternalProject)

ExternalProject_Add(zlib-ext
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source
        BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/binary/zlib
        CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/source/configure --prefix=${DIR_PROJECT_OUT}/${ANDROID_ABI}/zlib
        BUILD_COMMAND ${MAKE})

set(minizip-source ${CMAKE_CURRENT_SOURCE_DIR}/source/contrib/minizip)

ExternalProject_Add(minizip-ext
        SOURCE_DIR ${minizip-source}
        BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/binary/minizip
        CONFIGURE_COMMAND autoreconf -fvi ${minizip-source} && ${minizip-source}/configure --prefix=${DIR_PROJECT_OUT}/${ANDROID_ABI}/minizip
        BUILD_COMMAND ${MAKE})

add_library(zlib STATIC IMPORTED)
add_library(minizip STATIC IMPORTED)

add_dependencies(zlib zlib-ext)
add_dependencies(minizip minizip-ext)

set_target_properties(zlib PROPERTIES IMPORTED_LOCATION ${DIR_ROOT_OUT}/external/zlib/${ANDROID_ABI}/zlib/lib/libz.a)
set_target_properties(minizip PROPERTIES IMPORTED_LOCATION ${DIR_ROOT_OUT}/external/zlib/${ANDROID_ABI}/minizip/lib/libminizip.a)
