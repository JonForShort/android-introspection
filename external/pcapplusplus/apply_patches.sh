#!/usr/bin/env bash

set -x

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

patch -N -r - ${SCRIPT_DIR}/source/mk/platform.mk.linux ${SCRIPT_DIR}/patches/mk/platform.mk.linux.patch 

patch -N -r - ${SCRIPT_DIR}/source/mk/install.sh.template ${SCRIPT_DIR}/patches/mk/install.sh.template.patch 

set +x
