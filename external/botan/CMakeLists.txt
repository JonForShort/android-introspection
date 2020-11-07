cmake_minimum_required(VERSION 3.10.2)

include (ExternalProject)

if (WASM)
  set (BOTAN_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
  set (BOTAN_FLAGS --cc=clang --cpu=llvm --os=emscripten --prefix=${DIR_PROJECT_OUT})
  set (BOTAN_CONFIGURE ${BOTAN_SOURCE}/configure.py ${BOTAN_FLAGS})
else()
  set (BOTAN_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
  set (BOTAN_FLAGS --prefix=${DIR_PROJECT_OUT})
  set (BOTAN_CONFIGURE ${BOTAN_SOURCE}/configure.py ${BOTAN_FLAGS})
endif()

ExternalProject_Add (
  botan
  SOURCE_DIR ${BOTAN_SOURCE}
  CONFIGURE_COMMAND ${BOTAN_CONFIGURE}
  BUILD_COMMAND ${MAKE}
)
