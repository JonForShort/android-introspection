cmake_minimum_required(VERSION 3.10.2)

project(external)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set_property(GLOBAL PROPERTY GLOBAL_DEPENDS_DEBUG_MODE 0)

set(DIR_ROOT_OUT ${CMAKE_CURRENT_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE}/${CMAKE_ANDROID_ARCH_ABI})
set(DIR_ROOT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp)
set(DIR_ROOT_EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/../../../external)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIR_ROOT_OUT}/${CMAKE_PROJECT_NAME}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIR_ROOT_OUT}/${CMAKE_PROJECT_NAME}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIR_ROOT_OUT}/${CMAKE_PROJECT_NAME}/bin)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AndroidNdk.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PcapPlusPlus.cmake)

add_subdirectory(${DIR_ROOT_EXTERNAL} external)
