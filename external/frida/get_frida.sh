#!/bin/bash                                                                                                                                       
ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

FRIDA_BASE_URL=https://github.com/frida/frida/releases/download/_VERSION_/frida-gumjs-devkit-_VERSION_-android-_ARCH_.tar.xz
FRIDA_VERSION_TAG=_VERSION_
FRIDA_ARCH_TAG=_ARCH_

main() {
    frida_version=12.4.7
    if [ -n "$1" ]; then
        frida_version=$1
    fi

    download_dir=${ROOT_DIR}/${frida_version}
    if [ -n "$2" ]; then
        download_dir=$2
    fi

    if [ -d ${download_dir} ]; then
        echo ""
        echo "frida already exists; ${download_dir}"
        echo ""
        exit 0;
    fi

    rm -rf ${download_dir}
    mkdir -p ${download_dir}

    declare -A abis
    abis['x86']='x86'
    abis['x86_64']='x86_64'
    abis['arm']='armeabi-v7a'
    abis['arm64']='arm64-v8a'

    for frida_abi in "${!abis[@]}"; do
        android_abi=${abis["${frida_abi}"]}

        temp_dir=$(mktemp -d)
        temp_file=${temp_dir}/${frida_abi}.tar.xz
        dest_dir=${download_dir}/${android_abi}
        
        frida_url="${FRIDA_BASE_URL//$FRIDA_VERSION_TAG/$frida_version}"
        frida_url="${frida_url//$FRIDA_ARCH_TAG/$frida_abi}"

        wget ${frida_url} -O ${temp_file} --quiet
        
        mkdir -p ${dest_dir}
        tar xfJ ${temp_file} -C ${dest_dir}

        mkdir -p ${dest_dir}/include
        mv ${dest_dir}/*.h ${dest_dir}/include

        mkdir -p ${dest_dir}/lib
        mv ${dest_dir}/*.a ${dest_dir}/lib

        mkdir -p ${dest_dir}/example
        mv ${dest_dir}/*.c ${dest_dir}/example
    done

    echo ""
    echo "downloaded frida successfully; ${download_dir}"
    echo ""

    exit 0
}

main "$@"
