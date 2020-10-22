#!/usr/bin/env bash

set -x

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

ROOT_DIR=${SCRIPT_DIR}/../..

LOGS_DIR=${ROOT_DIR}/logs

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

    echo AI_UID=$(id -u $USER):$(id -g $USER) > ${ROOT_DIR}/.env
}

ai_setup_vscode()
{
    mkdir -p ${ROOT_DIR}/web_app/app/.vscode

    pushd ${ROOT_DIR}/web_app/app/.vscode

    ln -s $(realpath --relative-to=${ROOT_DIR}/web_app/app/.vscode ${ROOT_DIR}/dev/vscode)/launch_app.json launch.json

    popd

    mkdir -p ${ROOT_DIR}/web_app/wasm/.vscode

    pushd ${ROOT_DIR}/web_app/wasm/.vscode

    ln -s $(realpath --relative-to=${ROOT_DIR}/web_app/wasm/.vscode ${ROOT_DIR}/dev/vscode)/launch_wasm.json launch.json
    popd
}

ai_build()
{
    pushd ${ROOT_DIR}

    rm -rf ${LOGS_DIR}

    mkdir -p ${LOGS_DIR}

    ENV_SCRIPT=./dev/scripts/env.sh

    docker-compose build                                                     &> ${LOGS_DIR}/build.txt           && \
    docker-compose run android ${ENV_SCRIPT} ai_build_android                &> ${LOGS_DIR}/build_android.txt   && \
    docker-compose run web_app ${ENV_SCRIPT} ai_build_wasm                   &> ${LOGS_DIR}/build_wasm_pre.txt  && \
    docker-compose run web_app rm -rf web_app/wasm/out/wasm/external/minizip &> ${LOGS_DIR}/build_wasm_rm.txt   && \
    docker-compose run web_app ${ENV_SCRIPT} ai_build_wasm                   &> ${LOGS_DIR}/build_wasm_post.txt && \
    docker-compose run web_app ${ENV_SCRIPT} ai_build_wasm_host              &> ${LOGS_DIR}/build_wasm_host.txt && \
    docker-compose run web_app ${ENV_SCRIPT} ai_dist_wasm                    &> ${LOGS_DIR}/dist_wasm.txt       && \
    docker-compose run web_app ${ENV_SCRIPT} ai_build_webapp                 &> ${LOGS_DIR}/build_webapp.txt

    rm -rf archive

    mkdir -p archive

    tar cvzf archive/logs.tar.gz ${LOGS_DIR}
    tar cvzf archive/web_app.tar.gz web_app/app/dist
    tar cvzf archive/android.tar.gz android/app/build/outputs

    popd
}

ai_build_android()
{(
    pushd ${ROOT_DIR}/android

    ./gradlew clean build

    popd
)}

ai_build_wasm()
{(
    pushd ${ROOT_DIR}/external/wasm/emsdk

    EMSDK_TOOL=latest

    ./emsdk activate ${EMSDK_TOOL}

    if [ $? -ne 0 ]
    then
	./emsdk install ${EMSDK_TOOL} && ./emsdk activate ${EMSDK_TOOL}
    fi

    source ./emsdk_env.sh

    popd

    BUILD_DIR=${ROOT_DIR}/web_app/build/wasm

    mkdir -p ${BUILD_DIR}

    pushd ${BUILD_DIR}

    emcmake cmake -DWASM=True -DCMAKE_BUILD_TYPE=Debug --build ${ROOT_DIR}/web_app/wasm

    emmake make "$@"

    popd
)}

ai_build_wasm_host()
{(
    BUILD_DIR=${ROOT_DIR}/web_app/build/host

    mkdir -p ${BUILD_DIR}

    pushd ${BUILD_DIR}

    cmake -DWASM=False -DCMAKE_BUILD_TYPE=Debug --build ${ROOT_DIR}/web_app/wasm

    make "$@" && make test

    popd
)}

ai_build_webapp()
{(
    BUILD_DIR=${ROOT_DIR}/web_app/app

    pushd ${BUILD_DIR}

    NG_CLI_ANALYTICS=false npm install @angular/cli

    ./node_modules/.bin/ng build

    popd
)}

ai_dist_wasm()
{(
    DIST_DIR=${ROOT_DIR}/web_app/app/src/assets/js/wasm

    mkdir -p ${DIST_DIR}

    cp ${ROOT_DIR}/web_app/wasm/out/wasm/wasm/bin/* ${DIST_DIR}

    echo ""
    echo "copied successfuly; files can be found at the following path"
    echo ""
    echo $(readlink -m ${DIST_DIR})
    echo ""
)}

ai_update_submodules()
{(
    pushd ${ROOT_DIR}

    git submodule update --recursive --remote

    popd
)}

ai_setup_environment

"$@"
