cmake_minimum_required(VERSION 3.10.2)

set(headers
    include/aidl/com/github/jonforshort/vpn/IVpnService.h
    include/aidl/com/github/jonforshort/vpn/BpVpnService.h
    include/aidl/com/github/jonforshort/vpn/BnVpnService.h
    include/aidl/com/github/jonforshort/vpn/IVpnServiceListener.h
    include/aidl/com/github/jonforshort/vpn/BpVpnServiceListener.h
    include/aidl/com/github/jonforshort/vpn/BnVpnServiceListener.h
)

set(sources
    com/github/jonforshort/vpn/IVpnService.cpp
    com/github/jonforshort/vpn/IVpnServiceListener.cpp
)

add_library(aidl STATIC ${sources} ${headers})

target_include_directories(aidl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(AndroidNdkModules)

find_library(binder-lib binder_ndk)

target_link_libraries(aidl ${binder-lib})
