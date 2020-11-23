cmake_minimum_required(VERSION 3.10.2)

include (ExternalProject)

set (PCAPPLUSPLUS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set (PCAPPLUSPLUS_CONFIGURE_FLAGS --libpcap-include-dir ${DIR_ROOT_OUT}/external/libpcap/include
                                  --libpcap-lib-dir ${DIR_ROOT_OUT}/external/libpcap/lib
                                  --install-dir ${DIR_PROJECT_OUT})

ExternalProject_Add (
    pcapplusplus
    URL ${PCAPPLUSPLUS_SOURCE_DIR}
    CONFIGURE_COMMAND ${PCAPPLUSPLUS_SOURCE_DIR}/configure-linux.sh ${PCAPPLUSPLUS_CONFIGURE_FLAGS}
    BUILD_IN_SOURCE 1
    BUILD_COMMAND
        ${CMAKE_COMMAND} -E env CXX=${CMAKE_CXX_COMPILER}
	${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER}
	${CMAKE_COMMAND} -E env AR=${ANDROID_AR}
        ${CMAKE_COMMAND} -E env AS=${ANDROID_ASM_COMPILER}
        ${CMAKE_COMMAND} -E env RANLIB=${ANDROID_TOOLCHAIN_PREFIX}ranlib${ANDROID_TOOLCHAIN_SUFFIX}
        ${CMAKE_COMMAND} -E env STRIP=${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-strip${ANDROID_TOOLCHAIN_SUFFIX}
        ${CMAKE_COMMAND} -E env NM=${ANDROID_TOOLCHAIN_PREFIX}nm${ANDROID_TOOLCHAIN_SUFFIX}
	make VERBOSE=1 libs
)

add_custom_target (
    pcapplusplus_apply_patches
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/apply_patches.sh
)

add_dependencies (pcapplusplus pcap)
add_dependencies (pcapplusplus pcapplusplus_apply_patches)
