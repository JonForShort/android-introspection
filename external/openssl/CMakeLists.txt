cmake_minimum_required(VERSION 3.10.2)

include (ExternalProject)

set (OPENSSL_FLAGS linux-generic64)
set (OPENSSL_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)

ExternalProject_Add (
  openssl
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source
  CONFIGURE_COMMAND CROSS_COMPILE="" ${OPENSSL_SOURCE}/Configure ${OPENSSL_FLAGS} --prefix=${DIR_PROJECT_OUT}
  BUILD_COMMAND ${MAKE}
)
