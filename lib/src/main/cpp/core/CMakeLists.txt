cmake_minimum_required(VERSION 3.10.2)

set(v8-include ${DIR_ROOT_EXTERNAL}/v8/${ANDROID_ABI}/include/v8)
set(v8-lib ${DIR_ROOT_EXTERNAL}/v8/${ANDROID_ABI}/lib)

set(frida-include ${DIR_ROOT_EXTERNAL}/frida/frida-gumjs/${ANDROID_ABI}/include)
set(frida-lib ${DIR_ROOT_EXTERNAL}/frida/frida-gumjs/${ANDROID_ABI}/lib)

set(source native-lib.cpp)

add_library(native-lib SHARED ${source})

add_dependencies(native-lib frida)

target_link_libraries(native-lib ${v8-lib}/libv8-7.0.a)
target_link_libraries(native-lib ${frida-lib}/libfrida-gumjs.a)
target_link_libraries(native-lib apk)
target_link_libraries(native-lib utils)

target_include_directories(native-lib PRIVATE ${frida-include})
target_include_directories(native-lib PRIVATE ${v8-include})
target_include_directories(native-lib PRIVATE apk)
target_include_directories(native-lib PRIVATE utils)
