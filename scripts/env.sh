#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

ROOT_DIR=${SCRIPT_DIR}/..

BUILD_DIR=${ROOT_DIR}/build/wasm

ai_build() {

    ai_build_android && ai_build_wasm
}

ai_build_android() {
    (
	${ROOT_DIR}/gradlew clean build
    )
}

ai_build_wasm() {
    (
	source ${ROOT_DIR}/external/wasm/emsdk/emsdk_env.sh
	
	mkdir -p ${BUILD_DIR}
	
	pushd ${BUILD_DIR}
	
	emconfigure cmake --build ${ROOT_DIR}/lib
	
	emmake make "$@"
	
	popd
    )
}
