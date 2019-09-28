cmake_minimum_required(VERSION 3.10.2)

set(frida_version 12.4.7)

include(ExternalProject)

ExternalProject_Add(frida
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
        BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/get_frida.sh ${frida_version} ${CMAKE_CURRENT_SOURCE_DIR}/frida-gumjs
        INSTALL_COMMAND "")
