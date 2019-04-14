cmake_minimum_required (VERSION 3.10.2)

set (frida_version 12.4.7)

add_custom_target (frida
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/get_frida.sh ${frida_version} ${CMAKE_CURRENT_SOURCE_DIR}/out)
