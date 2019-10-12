cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_EXECUTABLE_SUFFIX ".wasm.js")

set(source wasm-lib.cpp)

add_executable(wasm-lib ${source})

target_include_directories(wasm-lib PRIVATE ${DIR_ROOT_EXTERNAL}/wasm/emscripten/system/include)

set_target_properties(wasm-lib PROPERTIES LINK_FLAGS "-s WASM=1")
