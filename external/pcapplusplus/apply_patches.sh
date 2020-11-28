#!/usr/bin/env bash

set -x

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

patch -N -r - ${SCRIPT_DIR}/source/mk/platform.mk.linux ${SCRIPT_DIR}/patches/mk/platform.mk.linux.patch 

patch -N -r - ${SCRIPT_DIR}/source/Packet++/Makefile ${SCRIPT_DIR}/patches/Packet++/Makefile.patch

patch -N -r - ${SCRIPT_DIR}/source/Common++/Makefile ${SCRIPT_DIR}/patches/Common++/Makefile.patch

patch -N -r - ${SCRIPT_DIR}/source/Common++/header/IpUtils.h ${SCRIPT_DIR}/patches/Common++/header/IpUtils.h.patch

patch -N -r - ${SCRIPT_DIR}/source/mk/PcapPlusPlus.mk.linux ${SCRIPT_DIR}/patches/mk/PcapPlusPlus.mk.linux.patch

set +x
