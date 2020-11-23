cmake_minimum_required(VERSION 3.10.2)

set(pcap-include ${DIR_ROOT_EXTERNAL}/libpcap/include)
set(pcap-lib     ${DIR_ROOT_EXTERNAL}/libpcap/lib)

set(pcapplusplus-include ${DIR_ROOT_EXTERNAL}/pcapplusplus/include)
set(pcapplusplus-lib     ${DIR_ROOT_EXTERNAL}/pcapplusplus/lib)

set(sources VpnService.cpp VpnServiceJni.cpp)

add_library(vpn SHARED ${sources})

target_include_directories(vpn PRIVATE util)
target_include_directories(vpn PRIVATE ${pcap-include})
target_include_directories(vpn PRIVATE ${pcapplusplus-include})

target_link_libraries(vpn utils)
target_link_libraries(vpn ${pcap-lib}/libpcap.a)
