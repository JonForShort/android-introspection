cmake_minimum_required (VERSION 3.10.2)

# v8

set (v8-include  ${DIR_ROOT_EXTERNAL}/v8/${ANDROID_ABI}/include/v8)
set (v8-lib      ${DIR_ROOT_EXTERNAL}/v8/${ANDROID_ABI}/lib)

# frida-gumjs

set (frida-include  ${DIR_ROOT_EXTERNAL}/frida/frida-gumjs/${ANDROID_ABI}/include)
set (frida-lib      ${DIR_ROOT_EXTERNAL}/frida/frida-gumjs/${ANDROID_ABI}/lib)

add_library (native-lib SHARED native-lib.cpp)

find_library (log-lib log)

add_dependencies (native-lib frida)

target_link_libraries (native-lib ${v8-lib}/libv8-7.0.a)
target_link_libraries (native-lib ${frida-lib}/libfrida-gumjs.a)
target_link_libraries (native-lib ${log-lib})

target_include_directories (native-lib PRIVATE ${frida-include})
target_include_directories (native-lib PRIVATE ${v8-include})
