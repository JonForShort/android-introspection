#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

ROOT_DIR=${SCRIPT_DIR}/..

ai_root_dir()
{
    pushd ${ROOT_DIR}
}

ai_setup_environment()
{
    if [[ -z "${AI_ANDROID_HOME}" ]]
    then
        if [[ -z "${ANDROID_HOME}" ]]
        then
            export AI_ANDROID_HOME=${ROOT_DIR}/external/android/sdk
        else
            export AI_ANDROID_HOME=${ANDROID_HOME}
        fi
    fi

    if [[ -z "${AI_TESTS_DIR}" ]]
    then
        export AI_TESTS_DIR=${ROOT_DIR}/web_app/wasm/test
    fi
}

ai_build()
{
    ai_build_android && ai_build_wasm && ai_build_host
}

ai_build_android()
{(
    ${ROOT_DIR}/android/gradlew clean build
)}

ai_build_web_app()
{(
    source ${ROOT_DIR}/external/wasm/emsdk/emsdk_env.sh

    BUILD_DIR=${ROOT_DIR}/web_app/build/wasm

    mkdir -p ${BUILD_DIR}

    pushd ${BUILD_DIR}

    emconfigure cmake -DWASM=True --build ${ROOT_DIR}/web_app/wasm

    emmake make "$@"

    popd
)}

ai_build_web_app_host()
{(
    BUILD_DIR=${ROOT_DIR}/web_app/build/host

    mkdir -p ${BUILD_DIR}

    pushd ${BUILD_DIR}

    cmake -DWASM=False -DCMAKE_BUILD_TYPE=Debug --build ${ROOT_DIR}/web_app/wasm

    make "$@" && make test

    popd
)}

ai_update_submodules()
{(
    git submodule update --recursive --remote
)}

ai_setup_environment

"$@"
