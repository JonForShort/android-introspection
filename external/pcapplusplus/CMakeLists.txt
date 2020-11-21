cmake_minimum_required(VERSION 3.10.2)

include (ExternalProject)

set (PCAPPLUSPLUS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set (PCAPPLUSPLUS_CONFIGURE_FLAGS --libpcap-include-dir ${CMAKE_CURRENT_SOURCE_DIR}/../libpcap
                                  --libpcap-lib-dir ${DIR_ROOT_OUT}/external/libpcap/lib
                                  --install-dir ${DIR_PROJECT_OUT})

ExternalProject_Add (
  pcapplusplus
  URL ${PCAPPLUSPLUS_SOURCE_DIR}
  PATCH_COMMAND mkdir -p ${DIR_PROJECT_OUT}
  CONFIGURE_COMMAND ${PCAPPLUSPLUS_SOURCE_DIR}/configure-linux.sh ${PCAPPLUSPLUS_CONFIGURE_FLAGS}
  BUILD_IN_SOURCE 1
)
