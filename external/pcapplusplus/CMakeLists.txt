cmake_minimum_required(VERSION 3.10.2)

include (ExternalProject)

set (PCAPPLUSPLUS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set (PCAPPLUSPLUS_CONFIGURE_FLAGS --libpcap-include-dir ${DIR_ROOT_OUT}/external/libpcap/include
                                  --libpcap-lib-dir ${DIR_ROOT_OUT}/external/libpcap/lib
                                  --install-dir ${DIR_PROJECT_OUT})

if (NOT DEFINED PCAPPLUSPLUS_CXX)
    set (PCAPPLUSPLUS_CXX "g++")
endif()

if (NOT DEFINED PCAPPLUSPLUS_CC)
    set (PCAPPLUSPLUS_CC "gcc")
endif()

if (NOT DEFINED PCAPPLUSPLUS_AR)
    set (PCAPPLUSPLUS_AR "ar")
endif()

ExternalProject_Add (
    pcapplusplus
    URL ${PCAPPLUSPLUS_SOURCE_DIR}
    CONFIGURE_COMMAND ${PCAPPLUSPLUS_SOURCE_DIR}/configure-linux.sh ${PCAPPLUSPLUS_CONFIGURE_FLAGS}
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make CXX=${PCAPPLUSPLUS_CXX} CC=${PCAPPLUSPLUS_CC} AR=${PCAPPLUSPLUS_AR} libs
)

add_custom_target (
    pcapplusplus_apply_patches
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/apply_patches.sh
)

add_dependencies (pcapplusplus pcap)
add_dependencies (pcapplusplus pcapplusplus_apply_patches)
