cmake_minimum_required(VERSION 3.10.2)

set(source wasm-lib.cpp)

add_library(wasm-lib SHARED ${source})

target_link_libraries(wasm-lib core)

target_include_directories(wasm-lib PRIVATE core)

