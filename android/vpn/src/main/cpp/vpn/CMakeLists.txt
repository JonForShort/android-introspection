cmake_minimum_required(VERSION 3.10.2)

set(sources VpnService.cpp VpnServiceJni.cpp)

add_library(vpn SHARED ${sources})

target_include_directories(vpn PRIVATE util)

target_link_libraries(vpn utils)
