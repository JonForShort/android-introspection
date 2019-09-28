cmake_minimum_required(VERSION 3.10.2)

set(LIB_PROJECTS core apk utils)

foreach (LIB_PROJECT ${LIB_PROJECTS})

    set(DIR_PROJECT_OUT ${DIR_ROOT_OUT}/${LIB_PROJECT})

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIR_PROJECT_OUT}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIR_PROJECT_OUT}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIR_PROJECT_OUT}/bin)

    add_subdirectory(${LIB_PROJECT})

endforeach ()
