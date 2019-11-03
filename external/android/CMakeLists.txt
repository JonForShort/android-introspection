cmake_minimum_required(VERSION 3.10.2)

add_custom_target(android-sdk COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in)
