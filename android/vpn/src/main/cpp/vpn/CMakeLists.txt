cmake_minimum_required(VERSION 3.10.2)

set(sources VpnService.cpp VpnServiceJni.cpp)

add_library(vpn SHARED ${sources})

target_include_directories(vpn PRIVATE spdlog)

target_link_libraries(vpn spdlog)

include(AndroidNdkModules)
android_ndk_import_module_cpufeatures()

find_library(log-lib log)

target_link_libraries(vpn cpufeatures)
target_link_libraries(vpn ${log-lib})
