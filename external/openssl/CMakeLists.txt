cmake_minimum_required(VERSION 3.10.2)

include (ExternalProject)

if (WASM)
  set (OPENSSL_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
  set (OPENSSL_FLAGS linux-generic64 -static --prefix=${DIR_PROJECT_OUT})
  set (OPENSSL_CONFIGURE CROSS_COMPILE="" ${OPENSSL_SOURCE}/Configure ${OPENSSL_FLAGS})
else()
  set (OPENSSL_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
  set (OPENSSL_FLAGS --prefix=${DIR_PROJECT_OUT})
  set (OPENSSL_CONFIGURE ${OPENSSL_SOURCE}/config ${OPENSSL_FLAGS})
endif()

ExternalProject_Add (
  openssl
  SOURCE_DIR ${OPENSSL_SOURCE}
  CONFIGURE_COMMAND ${OPENSSL_CONFIGURE}
  BUILD_COMMAND ${MAKE}
)
