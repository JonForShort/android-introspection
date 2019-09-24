cmake_minimum_required (VERSION 3.10.2)

include (ExternalProject)

ExternalProject_Add (
  zlib
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/source/configure --prefix=${DIR_PROJECT_OUT}/${ANDROID_ABI}/zlib
  BUILD_COMMAND ${MAKE}
)

set (minizip-source ${CMAKE_CURRENT_SOURCE_DIR}/source/contrib/minizip)

ExternalProject_Add (
  minizip
  SOURCE_DIR ${minizip-source}
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND autoreconf -fvi ${minizip-source} && ${minizip-source}/configure --prefix=${DIR_PROJECT_OUT}/${ANDROID_ABI}/minizip
  BUILD_COMMAND ${MAKE}
)
