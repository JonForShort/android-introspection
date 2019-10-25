cmake_minimum_required(VERSION 3.10.2)

include (ExternalProject)

ExternalProject_Add (
  openssl
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source
  CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/source/config --prefix=${DIR_PROJECT_OUT}
  BUILD_COMMAND ${MAKE}
)
