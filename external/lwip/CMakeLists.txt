cmake_minimum_required(VERSION 3.10.2)

add_subdirectory(source)

add_custom_target(
    ai_lwip_copy_headers
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${DIR_PROJECT_OUT}/include
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/source/src/include ${DIR_PROJECT_OUT}/include
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/include ${DIR_PROJECT_OUT}/include
)

add_custom_target(ai_lwip DEPENDS ai_lwip_copy_headers lwipcore)

add_dependencies(lwipcore ai_lwip_copy_headers)

target_include_directories(lwipcore PRIVATE ${DIR_PROJECT_OUT}/include)

