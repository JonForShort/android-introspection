cmake_minimum_required(VERSION 3.10.2)

if (ANDROID)

    include(AndroidNdkModules)
    android_ndk_import_module_cpufeatures()

    find_library(log-lib log)

    target_link_libraries(utils cpufeatures)
    target_link_libraries(utils ${log-lib})

endif ()
