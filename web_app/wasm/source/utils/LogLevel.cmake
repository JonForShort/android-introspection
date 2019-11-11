cmake_minimum_required(VERSION 3.10.2)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(log-level 1)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(log-level 3)
else ()
    set(log-level 0)
endif ()

target_compile_definitions(utils PUBLIC LOG_LEVEL=${log-level})
