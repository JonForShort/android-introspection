cmake_minimum_required(VERSION 3.10.2)

set(source main.cpp)

add_executable(wasm ${source})

target_link_libraries(wasm apk)
target_link_libraries(wasm utils)

target_include_directories(wasm PRIVATE apk)
target_include_directories(wasm PRIVATE utils)

if (WASM)

  add_definitions(-DWASM)

  set(CMAKE_EXECUTABLE_SUFFIX ".wasm.js")

  target_include_directories(wasm PRIVATE ${DIR_ROOT_EXTERNAL}/wasm/emscripten/system/include)

  set(WASM_EXCEPTION_FLAGS "-s EXCEPTION_DEBUG=1 -s DISABLE_EXCEPTION_CATCHING=0")

  set_target_properties(wasm PROPERTIES COMPILE_FLAGS ${WASM_EXCEPTION_FLAGS})

  set(WASM_EXTRA_EXPORTED_RUNTIME_METHODS "-s EXTRA_EXPORTED_RUNTIME_METHODS=\"['ccall','FS']\"")

  set_target_properties(wasm PROPERTIES LINK_FLAGS "--bind -s WASM=1 -s MODULARIZE=1 -s ENVIRONMENT='web' ${WASM_EXCEPTION_FLAGS} ${WASM_EXTRA_EXPORTED_RUNTIME_METHODS}")

endif()
