cmake_minimum_required(VERSION 3.10.2)

set (ANDROID_SDK_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sdk)
set (ANDROID_SDK_TOOLS_DIR ${ANDROID_SDK_ROOT_DIR}/tools)
set (ANDROID_SDK_FILE ${ANDROID_SDK_ROOT_DIR}/sdk.zip)
set (ANDROID_SDK_FILE_SHA256 92ffee5a1d98d856634e8b71132e8a95d96c83a63fde1099be3d86df3106def9)

if(EXISTS ${ANDROID_SDK_ROOT_DIR}/${ANDROID_SDK_FILE_SHA256})
  MESSAGE("android sdk already exists")
  return()
endif()

function(download_file url filename hash_type hash)

  if(EXISTS ${filename})
    MESSAGE("android sdk file already exists")
    return()
  endif()

  MESSAGE("downloading android sdk")

  file(DOWNLOAD ${url} ${filename}
    SHOW_PROGRESS
    TIMEOUT 60
    EXPECTED_HASH ${hash_type}=${hash}
    TLS_VERIFY ON)

  MESSAGE("finished downloading android sdk")

endfunction(download_file)

download_file(
  https://dl.google.com/android/repository/sdk-tools-linux-4333796.zip
  ${ANDROID_SDK_FILE}
  SHA256 ${ANDROID_SDK_FILE_SHA256})

if(NOT EXISTS ${ANDROID_SDK_TOOLS_DIR})

  MESSAGE("android sdk needs to be extracted")

  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar -xf ${ANDROID_SDK_FILE}
    WORKING_DIRECTORY ${ANDROID_SDK_ROOT_DIR}
    RESULT_VARIABLE zipped)

  MESSAGE("finished extracting android sdk = " ${zipped})
  
  if (zipped EQUAL "0")
    file(TOUCH ${ANDROID_SDK_ROOT_DIR}/${ANDROID_SDK_FILE_SHA256})
    file(REMOVE ${ANDROID_SDK_FILE})
  endif()

endif()
