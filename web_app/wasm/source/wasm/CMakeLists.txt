cmake_minimum_required(VERSION 3.10.2)

set(source main.cpp)

add_executable(wasm ${source})

target_link_libraries(wasm apk)

target_include_directories(wasm PRIVATE apk)

if (WASM)

  add_definitions(-DWASM)

  set(CMAKE_EXECUTABLE_SUFFIX ".wasm.js")

  target_include_directories(wasm PRIVATE ${DIR_ROOT_EXTERNAL}/wasm/emscripten/system/include)

  set_target_properties(wasm PROPERTIES LINK_FLAGS "--bind -s WASM=1 -s MODULARIZE=1 -s ENVIRONMENT='web'")

endif()
