#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

ROOT_DIR=${SCRIPT_DIR}/..

BUILD_DIR=${ROOT_DIR}/build/wasm

ai_build() {

    ai_build_android && ai_build_wasm
}

ai_build_android() {

    ${ROOT_DIR}/gradlew clean build
}

ai_build_wasm() {

    mkdir -p ${BUILD_DIR}

    pushd ${BUILD_DIR}

    cmake -DCMAKE_TOOLCHAIN_FILE=${ROOT_DIR}/external/wasm/emscripten/cmake/Modules/Platform/Emscripten.cmake \
	  -DCMAKE_BUILD_TYPE=DEBUG \
	  --target wasm-lib \
	  --build ${ROOT_DIR}/lib

    make "$@"

    popd
}
