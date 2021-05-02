cmake_minimum_required(VERSION 3.10.2)

set(pcap-include ${DIR_ROOT_EXTERNAL}/libpcap/include)
set(pcap-lib ${DIR_ROOT_EXTERNAL}/libpcap/lib)

set(pcapplusplus-include ${DIR_ROOT_EXTERNAL}/pcapplusplus/include)
set(pcapplusplus-lib ${DIR_ROOT_EXTERNAL}/pcapplusplus/lib)

set(lwip-include ${DIR_ROOT_EXTERNAL}/lwip/include)
set(lwip-lib ${DIR_ROOT_EXTERNAL}/lwip/lib)

set(headers LocalVpnService.h VpnService.h VpnConnection.h)
set(sources LocalVpnService.cpp VpnService.cpp VpnConnection.cpp)

add_library(vpn SHARED ${sources} ${headers})

target_include_directories(vpn PRIVATE util)
target_include_directories(vpn PRIVATE aidl)
target_include_directories(vpn PRIVATE ${pcap-include})
target_include_directories(vpn PRIVATE ${pcapplusplus-include})
target_include_directories(vpn PRIVATE ${lwip-include})

target_link_libraries(vpn utils)
target_link_libraries(vpn aidl)
target_link_libraries(vpn ${pcap-lib}/libpcap.a)
target_link_libraries(vpn ${pcapplusplus-lib}/libPacket++.a)
target_link_libraries(vpn ${pcapplusplus-lib}/libCommon++.a)
target_link_libraries(vpn ${pcapplusplus-lib}/libPcap++.a)
target_link_libraries(vpn ${lwip-lib}/liblwipcore.so)
