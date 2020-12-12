cmake_minimum_required(VERSION 3.10.2)

add_subdirectory (source)

add_custom_target (
    ai_pcap
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/source/pcap/ ${DIR_PROJECT_OUT}/include/pcap
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/source/pcap.h ${DIR_PROJECT_OUT}/include/pcap.h
    DEPENDS pcap_static
)
