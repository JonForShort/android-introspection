#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

ROOT_DIR=${SCRIPT_DIR}/..

BUILD_DIR=${ROOT_DIR}/build

ai_build() {
    ai_build_android && ai_build_wasm
}

ai_build_android()
{
    (
	${ROOT_DIR}/gradlew clean build
    )
}

ai_build_wasm()
{
    (
	source ${ROOT_DIR}/external/wasm/emsdk/emsdk_env.sh
	
	WASM_BUILD_DIR=${BUILD_DIR}/wasm
	
	mkdir -p ${WASM_BUILD_DIR}
	
	pushd ${WASM_BUILD_DIR}
	
	emconfigure cmake -DWASM=True --build ${ROOT_DIR}/lib
	
	emmake make "$@"
	
	popd
    )
}

ai_build_host()
{
    (
	mkdir -p ${BUILD_DIR}

	pushd ${BUILD_DIR}

	cmake -DHOST=True -DCMAKE_BUILD_TYPE=Debug --build ${ROOT_DIR}/lib

	make "$@"

	popd
    )
}
