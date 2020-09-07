cmake_minimum_required(VERSION 3.10.2)

set(source
  apk.cpp
  apk_parser.cpp
  binary_xml/binary_xml.cpp
  binary_xml/binary_xml_element.cpp
  binary_xml/binary_xml_visitor.cpp
  binary_xml/string_xml_visitor.cpp
  binary_xml/attributes_getter_visitor.cpp
  binary_xml/attributes_setter_visitor.cpp
  zip_archiver.cpp
)

add_library(apk STATIC ${source})

target_link_libraries(apk minizip)
target_link_libraries(apk gtest)
target_link_libraries(apk utils)

target_include_directories(apk PRIVATE minizip)
target_include_directories(apk PRIVATE gtest)
target_include_directories(apk PRIVATE utils)

target_include_directories(apk PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(apk PRIVATE ${DIR_ROOT_EXTERNAL}/minizip/source)

if (WASM)

  #
  # Adding Exception Support
  #

  set(WASM_EXCEPTION_FLAGS "-s EXCEPTION_DEBUG=1 -s DISABLE_EXCEPTION_CATCHING=0")

  set_target_properties(apk PROPERTIES COMPILE_FLAGS ${WASM_EXCEPTION_FLAGS})

endif()

if (NOT WASM)

  #
  # Adding Tests
  #

  add_executable(apk_test apk_test.cpp)

  target_link_libraries(apk_test apk)
  target_link_libraries(apk_test test_apk_analyzer)
  target_link_libraries(apk_test gtest_main)

  target_include_directories(apk PRIVATE test_apk_analyzer)

  add_test(NAME apk_test COMMAND apk_test)

  add_dependencies(apk_test android-sdk)

endif()
