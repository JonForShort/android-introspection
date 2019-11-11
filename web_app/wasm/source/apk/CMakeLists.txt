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

if (NOT WASM)

  #
  # tests
  #
  add_executable(apk_test apk_test.cpp)

  target_link_libraries(apk_test apk)
  target_link_libraries(apk_test gtest_main)

  add_test(NAME apk_test COMMAND apk_test)

  add_dependencies(apk_test android-sdk)

endif()
