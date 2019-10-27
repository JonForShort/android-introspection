cmake_minimum_required(VERSION 3.10.2)

project(utils CXX)

set(source
        include/utils/log.h
        include/utils/utils.h
        include/utils/v8_wrapper.h
        test.cpp)

add_library(utils STATIC ${source})

target_include_directories(utils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(log-level 1)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(log-level 3)
else ()
    set(log-level 0)
endif ()

target_compile_definitions(utils PUBLIC LOG_LEVEL=${log-level})

if (ANDROID)

    include(AndroidNdkModules)
    android_ndk_import_module_cpufeatures()

    find_library(log-lib log)

    target_link_libraries(utils cpufeatures)
    target_link_libraries(utils ${log-lib})

endif ()