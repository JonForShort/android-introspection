cmake_minimum_required(VERSION 3.10.2)

if (ANDROID)

    set(source android-lib.cpp)

    add_library(android-lib SHARED ${source})

    target_link_libraries(android-lib core)

    target_include_directories(android-lib PRIVATE core)

endif (ANDROID)