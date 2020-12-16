#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

aidl --lang=ndk --out=$SCRIPT_DIR --header_out=$SCRIPT_DIR/include \
     $SCRIPT_DIR/../../aidl/com/github/jonforshort/vpn/IVpnService.aidl \
     $SCRIPT_DIR/../../aidl/com/github/jonforshort/vpn/IVpnServiceListener.aidl
