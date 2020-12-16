cmake_minimum_required(VERSION 3.10.2)

if (ANDROID)

    set(EXTERNAL_PROJECTS android spdlog libpcap pcapplusplus)

elseif (WASM OR WASM_HOST)

    set(EXTERNAL_PROJECTS boost frida googletest minizip botan spdlog pugixml zlib)

endif ()

foreach (EXTERNAL_PROJECT ${EXTERNAL_PROJECTS})

    set(DIR_PROJECT_OUT ${DIR_ROOT_OUT}/external/${EXTERNAL_PROJECT})

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIR_PROJECT_OUT}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIR_PROJECT_OUT}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIR_PROJECT_OUT}/bin)

    file(MAKE_DIRECTORY ${DIR_PROJECT_OUT})

    add_subdirectory(${EXTERNAL_PROJECT})

endforeach ()
