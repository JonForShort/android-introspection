cmake_minimum_required(VERSION 3.10.2)

set(source apk.cpp)

add_library(apk STATIC ${source})

target_link_libraries(apk minizip)
target_link_libraries(apk gtest)
target_link_libraries(apk utils)

target_include_directories(apk PRIVATE minizip)
target_include_directories(apk PRIVATE gtest)
target_include_directories(apk PRIVATE utils)

target_include_directories(apk PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(apk PRIVATE ${DIR_ROOT_EXTERNAL}/minizip/source)

#
# tests
#
add_executable(apk_test apk_test.cpp)

target_link_libraries(apk_test gtest_main)

set(tool_apk_analyzer ${DIR_ROOT_OUT}/external/android/sdk/bin/apkanalyzer)

add_test(NAME apk_test COMMAND apk_test ${tool_apk_analyzer} ${PROJECT_SOURCE_DIR}/test/apk/test_release.apk)

add_dependencies(apk_test android-sdk)
