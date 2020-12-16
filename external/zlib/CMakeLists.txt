cmake_minimum_required(VERSION 3.10.2)

option (SKIP_EXAMPLE_FILES "Build example files" OFF)

set (SKIP_EXAMPLE_FILES ON)

add_subdirectory(source)

add_custom_target (
    ai_zlib
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_PROJECT_OUT}/include
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/source/zlib.h ${DIR_PROJECT_OUT}/include/zlib.h
    DEPENDS zlib
)
