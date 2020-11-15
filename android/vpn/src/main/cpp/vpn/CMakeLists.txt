cmake_minimum_required(VERSION 3.10.2)

set(sources VpnService.cpp VpnServiceJni.cpp)

add_library(vpn SHARED ${sources})
