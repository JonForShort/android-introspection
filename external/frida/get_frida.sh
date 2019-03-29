#!/bin/bash                                                                                                                                       
ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

FRIDA_BASE_URL=https://github.com/frida/frida/releases/download/_VERSION_/frida-gumjs-devkit-_VERSION_-android-_ARCH_.tar.xz
FRIDA_VERSION_TAG=_VERSION_
FRIDA_ARCH_TAG=_ARCH_

main() {
    frida_version=12.4.5
    if [ -n "$1" ]; then
	frida_version=$1
    fi

    download_dir=${ROOT_DIR}/${frida_version}
    rm -rf ${download_dir}
    mkdir -p ${download_dir}

    for arch in x86 x86_64 arm arm64; do
	temp_dir=$(mktemp -d)
	temp_file=${temp_dir}/${arch}.tar.xz
	dest_dir=${download_dir}/${arch}
	
	frida_url="${FRIDA_BASE_URL//$FRIDA_VERSION_TAG/$frida_version}"
	frida_url="${frida_url//$FRIDA_ARCH_TAG/$arch}"

	wget ${frida_url} -O ${temp_file} --quiet
	
	mkdir -p ${dest_dir}
	tar xvfJ ${temp_file} -C ${dest_dir}

	mkdir -p ${dest_dir}/include
	mv ${dest_dir}/*.h ${dest_dir}/include

	mkdir -p ${dest_dir}/lib
	mv ${dest_dir}/*.a ${dest_dir}/lib

	mkdir -p ${dest_dir}/example
	mv ${dest_dir}/*.c ${dest_dir}/example
    done
}

main "$@"
